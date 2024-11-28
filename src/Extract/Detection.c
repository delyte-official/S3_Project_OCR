#include <err.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>


#define FLAG_IS_GRID (1 << 0)
#define FLAG_IS_WORD (1 << 1)
#define FLAG_UNUSED 0

//Cluster struct
typedef struct cluster {
    int size;
    int centerX, centerY;
    int maxX, maxY, minX, minY;
    struct cluster *next;
    int flags;
} Cluster;


//Line struct
typedef struct {
    int pos, size, nbr;
} Line;


//Size struct
typedef struct {
    int x, y;
} Size;


/* is_black():
    Returns if a pixel is black.
*/
int is_black(guchar *p) {
    return (p[0] == 0) && (p[1] == 0) && (p[2]==0);
}


/* dfs():
    Adds pixels to an initialized cluster.
*/
void dfs(Cluster *end, int x, int y, gboolean** visit,
        GdkPixbuf *pixbuf) {
    end->size++;
    if (x > end->maxX)
        end->maxX = x;
    else if (x < end->minX)
        end->minX = x;
    if (y > end->maxY)
        end->maxY = y;
    else if (y < end->minY)
        end->minY = y;
    //Marking visited
    visit[x][y] = TRUE;
    //Recursive calls
    int height = gdk_pixbuf_get_height(pixbuf);
    int width = gdk_pixbuf_get_width(pixbuf);
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
    int N = gdk_pixbuf_get_n_channels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    for (int i = 0; i < 8; i++) { //Calculating new coordinates
        int new_x = i < 3 ? x-1 : i<5 ? x : x+1;
        int new_y = i < 3 ? y-1+i%3 : i<5 ? y+1+(i-4)*2: y-1+(i-2)%3;
        if (new_x < 0 || new_y < 0 || new_y >= height ||new_x >= width)
            continue;
        guchar *p = pixels + new_x * N + new_y*rowstride;
        if (visit[new_x][new_y] || !is_black(p))
            continue;
        dfs(end, new_x, new_y, visit, pixbuf);
    }
}


/* initCluster():
    Initialize a new cluster at the given pointer.
*/
void initCluster(Cluster **chain, int x, int y, gboolean** visit,
        GdkPixbuf *pixbuf) {
    Cluster *end = malloc(sizeof(Cluster));
    *end = (Cluster) {
        .size = 0,
        .centerX = 0, .centerY = 0,
        .maxX = x, .minX = x,
        .maxY = y, .minY = y,
        .next = NULL,
        .flags = 0
    };
    if (*chain != NULL) {
        (*chain)->next = end;
        *chain = (*chain)->next;
    } else
        *chain = end;
    dfs(end, x, y, visit, pixbuf);
    end->centerX = (end->minX + end->maxX)/2;
    end->centerY = (end->minY + end->maxY)/2;
}


/* retrieve_clusters():
    First step of the extraction.
    Retrieves all clusters of an image.
*/
int retrieve_clusters(GdkPixbuf *input, Cluster **last, int *median) {
    //Grab the size of the pixbuf
    int width = gdk_pixbuf_get_width(input);
    int height = gdk_pixbuf_get_height(input);

    //Array to mark pixels as visisted
    gboolean** visited = malloc(width * sizeof(gboolean*));
    if (visited == NULL)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < width; i++) {
        visited[i] = calloc(height, sizeof(gboolean));
        if (visited[i] == NULL) {
            free(visited);
            errx(EXIT_FAILURE, "calloc()");
        }
    }

    //Retrieving data form the input pixbuf
    int N = gdk_pixbuf_get_n_channels(input);
    int rowstride = gdk_pixbuf_get_rowstride(input);
    guchar* pixels = gdk_pixbuf_get_pixels(input);
    int clusterCount = 0;

    //Finding all clusters
    Cluster *start = NULL;
    Cluster *chain = NULL;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //new cluster?
            guchar *pixel = pixels + y * rowstride + x * N;
            if (is_black(pixel) && !visited[x][y]) {//0 means a black pixel
                initCluster(&chain, x, y, visited, input);
                if (start == NULL)
                    start = chain;
                *median+=chain->size;
                clusterCount++;
            }
        }
    }
    *last = start;
    //Updating median
    *median/=clusterCount;
    return clusterCount;
}


//JUST TO TEST - TO REMOVE
void testing(Cluster *first, GdkPixbuf *pixbuf, char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);
    
    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    while (first!=NULL) {
        //Coloring into red the whole cluster
        for (int x = first->minX; x <= first->maxX; x++) {
            p = pixels + first->minY*rowstride + x*N;
            p[0] = 255;p[1]=0;p[2]=0;
            p = pixels + first->maxY*rowstride +x*N;
            p[0] = 255;p[1]=0;p[2]=0;
        }
        for (int y = first->minY; y <= first->maxY; y++) {
            p = pixels + y*rowstride +first->minX*N;
            p[0] = 255;p[1]=0;p[2]=0;
            p = pixels + y*rowstride +first->maxX*N;
            p[0] = 255;p[1]=0;p[2]=0;
        }
        first=first->next;
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
}


/* threshold_filer():
    Filter all clusters according to a threshold.
*/
int threshold_filter(Cluster **clusters, int count, int median) {
    const int threshold = 0.85f * median;
    printf("Threshold: %d-%d\n",median-threshold,median+threshold);
    //Iterating
    Cluster *prev = NULL;
    Cluster *curr = *clusters;
    while (curr != NULL) {
        if (curr->size > threshold+median || curr->size < median-threshold) {
            printf("Size was: %d\n",curr->size);
            //Delete cluster
            if (prev == NULL)
                *clusters = curr->next;
            else
                prev->next = curr->next;
            Cluster *next = curr->next;
            free(curr);
            curr = next;
            count--;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return count;
}


/* expand_y:
    Expand the table on y by 1
*/
void expand_y(Cluster** *table, int xs, int *ys, int size) {
    int difference = size - *ys;
    *ys=size;
    for (int i = 0; i < xs; i++) {
        Cluster **tmp = realloc(table[i],sizeof(Cluster*)*(*ys));
        if (!tmp && size > 0)
            errx(EXIT_FAILURE, "realloc()");
        if (difference > 0) {
            for (int j = size-difference; j < size; j++)
                tmp[j]=NULL;
        }
        table[i] = tmp;
    }
}


/* expand_x():
    Expand the table on x by 1.
*/
void expand_x(Cluster** **table, int *xs, int ys, int size) {
    int difference = size-*xs;
    *xs=size;
    Cluster ***tmp= realloc(*table,sizeof(Cluster**)*(*xs));
    if (!tmp && size > 0)
        errx(EXIT_FAILURE, "realloc()");
    if (difference > 0) {
        for (int i = 0; i < difference; i++) {
            Cluster **tmp2 = calloc(sizeof(Cluster*),ys);
            if (!tmp2)
                errx(EXIT_FAILURE, "calloc()");
            tmp[size-difference+i]=tmp2;
        }
    }
    *table = tmp;
}


/* expand_line():
    Expand the array of "Line" to "size".
*/
void expand_line(Line* *line, int size) {
    Line *tmp = realloc(*line,sizeof(Line)*size);
    if (!tmp)
        errx(EXIT_FAILURE,"realloc()");
    memset(&tmp[size-1],0,sizeof(Line));
    *line = tmp;
}


/* addToLine():
    Add an element to the line information.
*/
void addToLine(Line *line, Cluster ***table, int index, int ys, int isHori) {
    int total_p = 0, total_s = 0;
    //Calculate real total values
    for (int i = 0; i < ys; i++) {
        if (table[index][i]) {
            if (isHori) {
                total_p+=table[index][i]->centerX;
                total_s+=table[index][i]->maxX-table[index][i]->minX;
            } else {
                total_p+=table[index][i]->centerY;
                total_s+=table[index][i]->maxY-table[index][i]->minY;
            }
        }
    }
    //update
    line->nbr++;
    line->pos = total_p / line->nbr;
    line->size = total_s / line->nbr;
}


//JUST DO TEST - //TODO: to REMOVE
void print_testing(Cluster ***rows, Cluster ***cols, int rows_x, int rows_y,
        int cols_x, int cols_y, Line* rows_inf, Line* cols_inf) {
    printf("\nPRINT HORIZONTAL ALIGNMENT:\n");
    for (int x = 0; x < rows_x; x++) {
        printf("%3d %3d ",rows_inf[x].pos,rows_inf[x].size);
        for (int y = 0; y < rows_y; y++)
            printf("%2c ",rows[x][y] != NULL ? 'X' : ' ');
        printf("\n");
    }
    printf("\nPRINT VERTICAL ALIGNMENT:\n");
    for (int x = 0; x < cols_x; x++)
        printf("%3d ",cols_inf[x].pos);
    printf("\n");
    for (int x = 0; x < cols_x; x++)
        printf("%3d ",cols_inf[x].size);
    printf("\n");
    for (int y = 0; y < cols_y; y++) {
        for (int x = 0; x < cols_x; x++) {
            printf("%3c ",cols[x][y] != NULL ? 'X' : ' ');
        }
        printf("\n");
    }
}


/* compareX():
    Compare property centerX of two clusters
*/
int compareX(const void *a, const void *b) {
    Cluster *cA = *(Cluster**)a;
    Cluster *cB = *(Cluster**)b;
    int resA = cA != NULL ? cA->centerX : INT_MAX;
    int resB = cB != NULL ? cB->centerX : INT_MAX;
    return resA < resB ? -1 : resA > resB ? 1 : 0;
}


/* compareY():
    Compare property centerY of two clusters.
*/
int compareY(const void *a, const void *b) {
    Cluster *cA = *(Cluster**)a;
    Cluster *cB = *(Cluster**)b;
    int resA = cA != NULL ? cA->centerY : INT_MAX;
    int resB = cB != NULL ? cB->centerY : INT_MAX;
    return resA < resB ? -1 : resA > resB ? 1 : 0;
}


/* align_clusters():
    Align clusters in two matrices: horizontally and vertically.
*/
void align_clusters(Cluster *clusters,Cluster ****matrixH,Cluster ****matrixV,
        Line **rows_p, Line **cols_p, Size *sizeH, Size *sizeV) {
    //STEP 1: INITIALIZATION
    //Create the SPACIAL CLUSTERING TABLE
    int rows_x = 0, rows_y = 0, cols_x = 0, cols_y = 0;
    Cluster* **rows = NULL;
    Cluster* **cols = NULL;
    //Creating the rows and cols average positions
    Line* row_avg = NULL;
    Line* col_avg = NULL;
    
    //STEP 2: Convert to Arrays and Sort
    int count = 0;
    Cluster *curr = clusters;
    while (curr) {
        count++;
        curr = curr->next;
    }
    Cluster **sortHori = malloc(sizeof(Cluster*)*count);
    Cluster **sortVert = malloc(sizeof(Cluster*)*count);
    curr = clusters;
    for (int i = 0; i < count; i++,curr=curr->next) {
        sortHori[i]=curr;
        sortVert[i]=curr;
    }
    qsort(sortHori, count, sizeof(Cluster*),compareX);
    qsort(sortVert, count, sizeof(Cluster*),compareY);

    //STEP 3: Iteration of clusters
    //ROWS MATRICE
    for (int i = 0; i < count; i ++) {
        curr = sortVert[i];
        ////Find the ROW coordinate [HORIZONTAL]
        int ROW = -1;
        for (int r_cmp = 0; r_cmp < rows_x; r_cmp++) {
            int TOLERANCE = row_avg[r_cmp].size /2;
            if (abs(curr->centerY - row_avg[r_cmp].pos) <= TOLERANCE) {
                //Add it to this horizontal line
                ROW = r_cmp;
                break;
            }
        }
        if (ROW == -1) { //Did not find: create a new line after
            expand_x(&rows, &rows_x,rows_y, rows_x+1);
            expand_line(&row_avg,rows_x);
            ROW = rows_x - 1;
        }
        //Detect if line is full
        if (rows[ROW][rows_y-1] != NULL)
            expand_y(rows,rows_x,&rows_y,rows_y+1);
        //Adding to ROW
        int add = 0;
        while (rows[ROW][add]!=NULL)
            add++;
        rows[ROW][add]=curr;

        addToLine(&(row_avg[ROW]),rows,ROW,rows_y,0);
    }
    //COLS MATRICE
    for (int i = 0; i < count; i++) {
        curr = sortHori[i];
        ////Find the COL coordinate [VERTICAL]
        int COL = -1;
        for (int c_cmp = 0; c_cmp < cols_x; c_cmp++) {
            int TOLERANCE = col_avg[c_cmp].size /2;
            if (abs(curr->centerX - col_avg[c_cmp].pos) <= TOLERANCE) {
                //Add it to this horizontal line
                COL = c_cmp;
                break;
            }
        }
        if (COL == -1) { //Did not find: create a new line after
            expand_x(&cols, &cols_x,cols_y,cols_x+1);
            expand_line(&col_avg,cols_x);
            COL = cols_x - 1;
        }
        //Detect if line is full
        if (cols[COL][cols_y-1] != NULL)
            expand_y(cols,cols_x,&cols_y,cols_y+1);
        //Adding to ROW
        int add = 0;
        while (cols[COL][add]!=NULL)
            add++;
        cols[COL][add]=curr;
        
        addToLine(&(col_avg[COL]),cols,COL,cols_y, 1);
    }
    //Sort the LINES of each matrix to put them in order
    for (int line = 0; line < rows_x; line++)
        qsort(rows[line],rows_y,sizeof(Cluster*),compareX);
    for (int line = 0; line < cols_x; line++)
        qsort(cols[line],cols_y,sizeof(Cluster*),compareY);

    //TESTING
    print_testing(rows,cols,rows_x,rows_y,cols_x,cols_y,row_avg,col_avg);
    //TO REMOVE
    *matrixH = rows;
    *matrixV = cols;
    *rows_p = row_avg;
    *cols_p = col_avg;
    *sizeH = (Size) {.x=rows_x,.y=rows_y};
    *sizeV = (Size) {.x=cols_x,.y=cols_y};
}


/* check_align():
    Check if two clusters are align in the SRC matrix.
*/
int check_align(Cluster ***SRC, Cluster *first, Cluster *second,
        Size size) {
    //Find the x coordinates of the two
    int x1 = -1;
    int x2 = -2; //Different values for the case they're not in the matrix
    for (int f = 0; f < size.x * size.y; f++) {
        int x = f / size.y, y = f % size.y;
        if (SRC[x][y] == NULL) {
            f += size.y-f % size.y - 1; //skip column
            continue;
        }
        if (SRC[x][y] == first) {
            x1 = x;
            if (x2>=0)
                break; //Found both
        } if (SRC[x][y] == second) {
            x2 = x;
            if (x1>=0)
                break; //found both
        }
    }
    return x1 == x2;
}

//TEST FUNCTION
void print_matrix(Cluster ***matrix, Size size) {
    printf("\nPRINT MATRIX(%d;%d):\n",size.x,size.y);
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++)
            printf("%c ", matrix[x][y]!=NULL ? 'X' : '\\');
        printf("\n");
    }
    printf("END\n\n");
}


/* find_grid():
    Tries to find the grid in the SRC matrix with help of CMP matrix.
*/
Size find_grid(Cluster ***SRC, Cluster ***CMP,
        Line *lines, Size sizeSRC, Size sizeCMP, Cluster ****DST) {
    Size sizeG = (Size) {.x=0,.y=0};
    *DST = NULL;
    ////FIND GRID
    //Search for grid in matrix HORIZONTAL
    for (int x = 0; x < sizeSRC.x; x++) {
        for (int y = 0; y < sizeSRC.y; y++) {
            if (SRC[x][y]==NULL)
                break; //row empty from now on
            sizeG.x = 0; sizeG.y = 0;
            //Begin finding for Cluster at (x;y)
            for (int i = y; i < sizeSRC.y && SRC[x][i] != NULL; i++)
                sizeG.x++;
            if (sizeG.x < 5)
                break; //Skip line
            int expand = sizeG.x; sizeG.x = 0; sizeG.y = 1;
            expand_x(DST,&sizeG.x,sizeG.y,expand);
            //Putting the first line into the grid matrix
            for (int i = 0; i < sizeG.x; i++)
                (*DST)[i][0]=SRC[x][y+i];
            for (int j = x+1; j < sizeSRC.x; j++) {
                int line_c = 0;
                int start_line = -1;
                for (int i = 0; i <= sizeSRC.y-5; i++) {
                    //Find the cluster of the row which align with previous
                    if (SRC[j][i] == NULL)
                        break;
                    if (check_align(CMP,SRC[x][y],SRC[j][i],
                                sizeCMP)) {
                        start_line = i;
                        break;
                    }
                }
                if (start_line==-1)
                    break;
                //Iterate from start_row
                for (int i = start_line; i-start_line<sizeG.x && i<sizeSRC.y;
                        i++) {
                    if (SRC[j][i] == NULL || !check_align(CMP,
                                SRC[x][y+i-start_line],SRC[j][i],sizeCMP)) {
                        if (line_c < sizeG.x && line_c>=5)
                            expand_x(DST,&sizeG.x,sizeG.y,line_c);
                        break;
                    }
                    line_c++;
                }
                if (line_c < 5)
                    break;
                expand_y(*DST,sizeG.x,&sizeG.y,sizeG.y+1);
                for (int i = 0; i < sizeG.x; i++)
                    (*DST)[i][j-x]=SRC[j][start_line+i];
            }
            if (sizeG.y >= 5)
                break;
            //Reset TRIAL
            expand_x(DST,&sizeG.x,sizeG.y,0);
        }
        if (sizeG.y >= 5)
            break;
    }
    if (sizeG.x >= 5 && sizeG.y >= 5)
        return sizeG;
    return (Size) {.x=-1,.y=-1};
}


void grid_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf, char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);

    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            Cluster *first = matrix[x][y];
            //Coloring into red the whole cluster
            for (int x = first->minX; x <= first->maxX; x++) {
                p = pixels + first->minY*rowstride + x*N;
                p[0] = 0;p[1]=255;p[2]=0;
                p = pixels + first->maxY*rowstride +x*N;
                p[0] = 0;p[1]=255;p[2]=0;
            }
            for (int y = first->minY; y <= first->maxY; y++) {
                p = pixels + y*rowstride +first->minX*N;
                p[0] = 0;p[1]=255;p[2]=0;
                p = pixels + y*rowstride +first->maxX*N;
                p[0] = 0;p[1]=255;p[2]=0;
            }
        }
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
}


/* classify_clusters():
    Classify clusters into the grid or word list. If it does not fit either
    of them, it gets deleted.
*/
void classify_clusters(Cluster ***matrixH, Cluster ***matrixV, Line *rows,
        Line *cols, Size sizeH, Size sizeV, GdkPixbuf *test) {
    Cluster ***grid;
    Size gridS = find_grid(matrixH,matrixV,rows,sizeH,sizeV,&grid);
    if (gridS.x==-1) {
        printf("Not found horizontally\n");
        gridS = find_grid(matrixV,matrixH,cols,sizeV,sizeH,&grid);
    }
    printf("Grid is: (%d;%d)\n",gridS.x,gridS.y);
    print_matrix(grid,gridS);
    //TEST RESULT
    grid_testing(grid,gridS,test,"grid_detect.png");
}


/* extract_information():
    Performs all the extraction from an image.
*/
void extract_information(GdkPixbuf *input) {
    //Linked list of clusters
    Cluster *start = NULL;

    //STEP 1: retrieving EVERY clusters
    int median_size = 0;
    int count = retrieve_clusters(input, &start, &median_size);
    printf("Median is: %d Count is : %d\n", median_size, count);
    testing(start,input,"iterate.png");
    //STEP 2: Filter clusters
    count = threshold_filter(&start, count, median_size);
    testing(start,input,"thresholdfilter.png");

    //STEP 3: Align clusters
    Cluster ***matrixH, ***matrixV;
    Line *rows, *cols;
    Size sizeH, sizeV;
    align_clusters(start, &matrixH, &matrixV, &rows, &cols,&sizeH,&sizeV);

    //STEP 4: Classify clusters
    classify_clusters(matrixH,matrixV,rows,cols,sizeH,sizeV,input);
}

int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf);
}
