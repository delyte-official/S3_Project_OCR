#include <err.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>


//Cluster struct
typedef struct cluster {
    int size;
    int centerX, centerY;
    int maxX, maxY, minX, minY;
    struct cluster *next;
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
        .next = NULL
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
    Cluster *test = start;
    //Updating median
    *median/=clusterCount;
    return clusterCount;
}


//JUST TO TEST - TO REMOVE
void testing(Cluster *first, int size, GdkPixbuf *pixbuf, char* filename) {
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
void expand_y(Cluster** *table, int xs, int *ys) {
    (*ys)++;
    for (int i = 0; i < xs; i++) {
        Cluster **tmp = realloc(table[i],sizeof(Cluster*)*(*ys));
        if (!tmp)
            errx(EXIT_FAILURE, "realloc()");
        tmp[*ys-1] = NULL;
        table[i] = tmp;
    }
}


/* expand_x():
    Expand the table on x by 1.
*/
void expand_x(Cluster** **table, Line* *line_avg, int *xs, int ys) {
    (*xs)++;
    Cluster ***tmp= realloc(*table,sizeof(Cluster**)*(*xs));
    Line *tmp2 = realloc(*line_avg, sizeof(Line)*(*xs));
    if (!tmp || !tmp2)
        errx(EXIT_FAILURE, "realloc()");
    Cluster **tmp3 = calloc(sizeof(Cluster*),ys);
    if (!tmp3)
        errx(EXIT_FAILURE, "calloc()");
    tmp[*xs-1]=tmp3;
    memset(&tmp2[*xs-1],0,sizeof(Line));
    *table = tmp; *line_avg = tmp2;
}


/* addToLine():
    Add an element to the line information.
*/
void addToLine(Line *line, Cluster ***table, int pos, int size,
        int index, int ys, int isHori) {
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
    return cA->centerX < cB->centerX ? -1 : cA->centerX > cB->centerX ? 1 : 0;
}


/* compareY():
    Compare property centerY of two clusters.
*/
int compareY(const void *a, const void *b) {
    Cluster *cA = *(Cluster**)a;
    Cluster *cB = *(Cluster**)b;
    return cA->centerY < cB->centerY ? -1 : cA->centerY > cB->centerY ? 1 : 0;
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
    printf("Iteration\n");
    //ROWS MATRICE
    for (int i = 0; i < count; i ++) {
        curr = sortVert[i];
        printf("New Cluster P(%d;%d)\n",curr->centerX,curr->centerY);
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
        printf("Row:%d\n",ROW);
        if (ROW == -1) { //Did not find: create a new line after
            expand_x(&rows, &row_avg, &rows_x,rows_y);
            ROW = rows_x - 1;
        }
        //Detect if line is full
        if (rows[ROW][rows_y-1] != NULL)
            expand_y(rows,rows_x,&rows_y);
        //Adding to ROW
        int add = 0;
        while (rows[ROW][add]!=NULL)
            add++;
        rows[ROW][add]=curr;

        printf("Insert in Row(%d)\n",ROW);
        addToLine(&(row_avg[ROW]),rows,curr->centerY,curr->maxY-curr->minY,
            ROW,rows_y,0);
        printf("Success\n\n");
    }
    //COLS MATRICE
    for (int i = 0; i < count; i++) {
        curr = sortHori[i];
        printf("New Cluster P(%d;%d)\n",curr->centerX,curr->centerY);
        ////Find the COL coordinate [VERTICAL]
        int COL = -1;
        for (int c_cmp = 0; c_cmp < cols_x; c_cmp++) {
            int TOLERANCE = col_avg[c_cmp].size /2;
            printf("Check[%d]: %d(T:%d;C:%d;AVG:%d)\n",c_cmp,abs(curr->centerX-col_avg[c_cmp].pos),TOLERANCE,curr->centerX,col_avg[c_cmp].pos);
            if (abs(curr->centerX - col_avg[c_cmp].pos) <= TOLERANCE) {
                //Add it to this horizontal line
                COL = c_cmp;
                break;
            }
        }
        printf("Col:%d\n",COL);
        if (COL == -1) { //Did not find: create a new line after
            expand_x(&cols, &col_avg, &cols_x,cols_y);
            COL = cols_x - 1;
        }
        //Detect if line is full
        if (cols[COL][cols_y-1] != NULL)
            expand_y(cols,cols_x,&cols_y);
        //Adding to ROW
        int add = 0;
        while (cols[COL][add]!=NULL)
            add++;
        cols[COL][add]=curr;
        
        printf("Insert in Col(%d)\n",COL);
        addToLine(&(col_avg[COL]),cols,curr->centerX,curr->maxX-curr->minX,
            COL,cols_y, 1);
        printf("Success\n\n");
    }

    //TESTING
    print_testing(rows,cols,rows_x,rows_y,cols_x,cols_y,row_avg,col_avg);
    *matrixH = rows;
    *matrixV = cols;
    *rows_p = row_avg;
    *cols_p = col_avg;
    *sizeH = (Size) {.x=rows_x,.y=rows_y};
    *sizeV = (Size) {.x=cols_x,.y=cols_y};
}


/* classify_clusters():
    Classify clusters into the grid and word list. If it is in nether, it
    is deleted.
*/
void classify_clusters(Cluster ***matrixH, Cluster ***matrixV,
        Line *rows, Line *cols, Size sizeH, Size sizeV) {
    Size sizeG = (Size) {.x=0,.y=0};
    ////FIND GRID
    //Search for grid in matrix HORIZONTAL
    for (int x = 0; x < sizeH.x; x++) {
        for (int y = 0; y < sizeH.y; y++) {
            if (matrixH[x][y]==NULL)
                break; //row empty from now on
            sizeG.x = 0; sizeG.y = 0;
            //Begin finding for Cluster at (x;y)
            printf("Start at (%d;%d) with H(%d;%d)\n",x,y,sizeH.x,sizeH.y);
            for (int i = y; i < sizeH.y && matrixH[x][i] != NULL; i++)
                sizeG.x++;
            if (sizeG.x < 5)
                break; //Skip row
            sizeG.y = 1;
            printf("SizeG.x:%d\n",sizeG.x);
            for (int j = x+1; j < sizeH.x; j++) {
                int row_c = 0;
                for (int i = y; i < sizeG.x+y; i++) {
                    printf("Test:%d;%d\n",j,i);
                    if (matrixH[j][i] == NULL) {
                        printf("NULL at (%d;%d)\n",j,i);
                        if (row_c < sizeG.x) {
                            if (row_c < 5)
                                goto break_grid;//Break(2)
                            sizeG.x = row_c;
                        }
                        break;
                    }
                    row_c++;
                }
                printf("+%d\n",row_c);
                sizeG.y++;
            }
            if (sizeG.y >= 5)
                goto found_grid;
            break_grid:; //LABEL= breaking the grid
            printf("Reset\nGrid trial: %d;%d\n",sizeG.x,sizeG.y);
        }
    }
    found_grid:;
    if (sizeG.x >= 5 && sizeG.y >= 5)
        printf("Found grid\n");
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
    testing(start,count,input,"iterate.png");
    //STEP 2: Filter clusters
    count = threshold_filter(&start, count, median_size);
    testing(start,count,input,"thresholdfilter.png");

    //STEP 3: Align clusters
    Cluster ***matrixH, ***matrixV;
    Line *rows, *cols;
    Size sizeH, sizeV;
    align_clusters(start, &matrixH, &matrixV, &rows, &cols,&sizeH,&sizeV);

    //STEP 4: Classify clusters
    classify_clusters(matrixH,matrixV,rows,cols,sizeH,sizeV);
}

int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf);
}
