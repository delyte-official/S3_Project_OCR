#define _GNU_SOURCE
#include <err.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>


#define FLAG_GRID 1
#define FLAG_WORD (1 << 1)
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


/* cut_from_pixbuf():
    Cut a pixbuf into a sub-pixbuf given two coordinates of a rectangle.
*/
GdkPixbuf* cut_from_pixbuf(GdkPixbuf *original, Size topL, Size botR)
{
    int width = botR.x - topL.x+1; //+1 because its a size
    int height = botR.y - topL.y+1;
    GdkPixbuf *subpix = gdk_pixbuf_new_subpixbuf(original, topL.x, topL.y,
            width, height);
    //Create copy bc "subpix" shares data with "original"
    GdkPixbuf *res = gdk_pixbuf_copy(subpix);
    g_object_unref(subpix);
    return res;
}


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
        errx(EXIT_FAILURE, "malloc(visited)");
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
        float density = curr->size/(float)((curr->maxX-curr->minX)*
                (curr->maxY-curr->minY));
        if (curr->size > threshold+median || curr->size < median-threshold
                || density < 0.2f) {
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
    if (ys==0)
        ys++;
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
    if (!sortHori || !sortVert)
        errx(EXIT_FAILURE, "malloc()");
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
        if (rows_y < 1 || rows[ROW][rows_y-1] != NULL)
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
        if (cols_y < 1 || cols[COL][cols_y-1] != NULL)
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
            for (int i = y; i < sizeSRC.y && SRC[x][i]!=NULL; i++)
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
                int maxPos = -1;
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
                    continue;
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
                    (*DST)[i][sizeG.y-1]=SRC[j][start_line+i];
            }
            if (sizeG.y >= 5)
                break;
            //Reset TRIAL
            expand_x(DST,&sizeG.x,sizeG.y,0);
        }
        if (sizeG.y >= 5)
            break;
    }
    if (sizeG.x < 1 || sizeG.y < 5)
        return (Size) {.x=-1,.y=-1};
    return sizeG;
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


void wordList_testing(Cluster ***matrix, Size size, GdkPixbuf *pixbuf,
        char* filename) {
    GdkPixbuf *res = gdk_pixbuf_copy(pixbuf);

    guchar* pixels = gdk_pixbuf_get_pixels(res);
    int N = gdk_pixbuf_get_n_channels(res);
    int rowstride = gdk_pixbuf_get_rowstride(res);

    guchar *p;
    //Iterating over the clusters
    for (int x = 0; x < size.x; x++) {
        //New word
        int r = (x*40)%256, g = (255-x*40)%256, b = 255;
        for (int y = 0; y < size.y; y++) {
            Cluster *first = matrix[x][y];
            if (first == NULL)
                break; //Skip row
            //Coloring into red the whole cluster
            for (int x = first->minX; x <= first->maxX; x++) {
                p = pixels + first->minY*rowstride + x*N;
                p[0] = r;p[1]=g;p[2]=b;
                p = pixels + first->maxY*rowstride +x*N;
                p[0] = r;p[1]=g;p[2]=b;
            }
            for (int y = first->minY; y <= first->maxY; y++) {
                p = pixels + y*rowstride +first->minX*N;
                p[0] = r;p[1]=g;p[2]=b;
                p = pixels + y*rowstride +first->maxX*N;
                p[0] = r;p[1]=g;p[2]=b;
            }
        }
    }
    gdk_pixbuf_save(res, filename, "png", NULL, NULL);
}


/* find_cluster():
    Returns the coordinates of a cluster in a cluster matrix.
*/
Size find_cluster(Cluster ***matrix, Size size, Cluster* tofind) {
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            if (matrix[x][y]==NULL)
                break; //Skip line
            if (matrix[x][y]==tofind)
                return (Size) {.x=x,.y=y};
        }
    }
    return (Size) {.x=-1,.y=-1};
}


/* find_wordlist():
    Detects the word list given two alignment matrices.
*/
Size find_wordlist(Cluster ***matrixH, Cluster***matrixV, Line *rows,
        Line *cols, Size sizeH, Size sizeV, Cluster ****wordList) {
    Size sizeW = (Size) {.x=0,.y=0};
    //Iteratove over the vertical matrix to find unflagged clusters
    for (int x = 0; x < sizeV.x; x++) {
        for (int y = 0; y < sizeV.y; y++) {
            if (matrixV[x][y]==NULL)
                break; //Skip column
            if (matrixV[x][y]->flags)
                continue; //Flagged => skip
            //Find its position in the horizontal matrix
            Size pos = find_cluster(matrixH,sizeH,matrixV[x][y]);
            //Grab the longest word from there
            int len = 1;
            for (int i = pos.y+1; i < sizeH.y && matrixH[pos.x][i]!=NULL;i++){
                int space = matrixH[pos.x][i]->centerX-
                    matrixH[pos.x][i-1]->centerX;
                int avg = matrixH[pos.x][i]->maxX-matrixH[pos.x][i]->minX +
                    matrixH[pos.x][i-1]->maxX-matrixH[pos.x][i-1]->minX;
                if (space > avg) //Too much space
                    break;
                len++;
            }
            if (len < 3) //Threshold for length of shortest word
                continue;
            expand_x(wordList,&sizeW.x,sizeW.y,sizeW.x+1);
            if (len+1>sizeW.y) //+1 for NULL terminator
                expand_y(*wordList,sizeW.x,&sizeW.y,len+1);
            //Save the values and flag the clusters
            for (int i = 0; i < len; i++) {
                (*wordList)[sizeW.x-1][i]=matrixH[pos.x][pos.y+i];
                matrixH[pos.x][pos.y+i]->flags |= FLAG_WORD;
            }
            (*wordList)[sizeW.x-1][len]=NULL; //Null terminator for the words
        }
    }
    print_matrix(*wordList,sizeW);
    return sizeW;
}


/* transpose_matrix():
    Transpose a matrix, X*Y => Y*X.
*/
void transpose_matrix(Cluster ****matrix, Size *size) {
    Cluster ***transposed = malloc(size->y*sizeof(Cluster**));
    if (!transposed)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < size->y; i++) {
        Cluster **tmp = malloc(size->x*sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "malloc()");
        transposed[i] = tmp;
    }

    //Transpose data
    for (int i = 0; i < size->x; i++) {
        for (int j = 0; j < size->y; j++) {
            transposed[j][i] = (*matrix)[i][j];
        }
        free((*matrix)[i]);
    }
    free(*matrix);
    *matrix = transposed;
    *size = (Size) {.x=size->y,.y=size->x};
}


/* filter_wordlist():
    Remove the groups of clusters that have been considered "words" but
    are not appart of the word list.
*/
void filter_wordlist(Cluster ****wordlist, Size *size, Cluster ***matrixH,
        Size sizeH, Cluster ***matrixV, Size sizeV) {
    ////Build alignement matrices
    int *calcH = calloc(sizeH.x,sizeof(int));
    int *calcV = calloc(sizeV.x,sizeof(int));
    if (!calcH || !calcV)
        errx(EXIT_FAILURE, "calloc()");
    for (int i = 0; i < size->x; i++) {
        //Horizontal alignment
        Size pos = find_cluster(matrixH,sizeH,(*wordlist)[i][0]);
        calcH[pos.x]++;
        //Vertical alignment
        pos = find_cluster(matrixV,sizeV,(*wordlist)[i][0]);
        calcV[pos.x]++;
    }
    Size sizeHN = (Size) {.x=0,.y=0};
    for (int i = 0; i < sizeH.x; i++) {
        if (calcH[i] == 0)
            continue;
        sizeHN.x++;
        if (calcH[i]>sizeHN.y)
            sizeHN.y=calcH[i];
    }
    Size sizeVN = (Size) {.x=0,.y=0};
    for (int i = 0; i < sizeV.x; i++) {
        if (calcV[i]==0)
            continue;
        sizeVN.x++;
        if (calcV[i]>sizeVN.y)
            sizeVN.y=calcV[i];
    }
    
    printf("Size found: H(%d;%d), V(%d;%d)\n",sizeHN.x,sizeHN.y,sizeVN.x,sizeVN.y);
    //Allocate matrices
    Cluster ***matrixHN = malloc(sizeof(Cluster**)*sizeHN.x);
    if (!matrixHN)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < sizeHN.x; i++) {
        Cluster **tmp = calloc(sizeHN.y,sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "calloc()");
        matrixHN[i]=tmp;
    }
    Cluster ***matrixVN = malloc(sizeof(Cluster**)*sizeVN.x);
    if (!matrixHN)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < sizeVN.x; i++) {
        Cluster **tmp = calloc(sizeVN.y,sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "calloc()");
        matrixVN[i] = tmp;
    }

    //Fill matrices
    for (int i = 0; i < size->x; i++) {
        //Horizontal alignment
        Size pos = find_cluster(matrixH,sizeH,(*wordlist)[i][0]);
        Size index = (Size) {.x=0,.y=0};
        for (int j = 0; j < pos.x; j++) {
            if (calcH[j]!=0)
                index.x++;
        }
        for (int j = 0; j < sizeHN.y && matrixHN[index.x][j]!=NULL; j++)
            index.y++;
        matrixHN[index.x][index.y] = (*wordlist)[i][0];
        //Vertical alignment
        pos = find_cluster(matrixV,sizeV,(*wordlist)[i][0]);
        index = (Size) {.x=0,.y=0};
        for (int j = 0; j < pos.x; j++) {
            if (calcV[j]!=0)
                index.x++;
        }
        for (int j = 0; j < sizeVN.y && matrixVN[index.x][j]!=NULL;j++)
            index.y++;
        matrixVN[index.x][index.y] = (*wordlist)[i][0];
    }
    free(calcH);
    free(calcV);
    //testing
    print_matrix(matrixHN, sizeHN);
    print_matrix(matrixVN, sizeVN);

    //NEXT STEP: filter out those too far in both matrices
    int upper_bd = -1, min = INT_MAX;
    for (int i = 1; i < sizeHN.x; i++) {
        int distance = matrixHN[i][0]->minY-matrixHN[i-1][0]->maxY;
        if (distance < min) {
            min = distance;
            upper_bd = i;
        }
    }
    int TOLERANCE = min * 1.2f;
    printf("Tolerance is: %d, with bounds %d to %d\n", TOLERANCE,upper_bd,upper_bd-1);
    int lower_bd = upper_bd-1;
    for (int i = lower_bd; i > 0; i--, lower_bd--) {
        int distance = matrixHN[i][0]->minY-matrixHN[i-1][0]->maxY;
        printf("Distance with upper word(%d):%d\n",i,distance);
        if (distance > TOLERANCE)
            break;
    }
    for (int i = upper_bd; i < sizeHN.x-1; i++, upper_bd++) {
        int distance = matrixHN[i+1][0]->minY-matrixHN[i][0]->maxY;
        if (distance > TOLERANCE)
            break;
    }
    printf("Found bounds: %d to %d\n",lower_bd,upper_bd);
    //Rebuild wordlist
    Size sizeWN = (Size) {.x=0,.y=0};
    int count_bullets = 0;
    for (int i = 0; i < size->x; i++) {
        Cluster *curr = (*wordlist)[i][0];
        Size pos = find_cluster(matrixHN,sizeHN,curr);
        if (pos.x > upper_bd || pos.x < lower_bd)
            continue;
        float density = (float)(curr->size)/
            ((curr->maxX-curr->minX+1)*(curr->maxY-curr->minY+1));
        if (density < 0.2f) //threshold between a fake and real letter
            count_bullets++;
        sizeWN.x++;
        int count = 0;
        for (int j = 0; j < size->y && (*wordlist)[i][j]!=NULL; j++)
            count++;
        if (count > sizeWN.y)
            sizeWN.y = count;
    }
    int start_index = 0;
    /*if (count_bullets > sizeWN.x /2) {
        sizeWN.y--;
        start_index=1;
    }*/
    sizeWN.y++; //Taking into account NULL pointer
    printf("New size of wordlist: %d;%d\n",sizeWN.x,sizeWN.y);
    Cluster ***new_wordlist = malloc(sizeof(Cluster**)*sizeWN.x);
    if (!new_wordlist)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < sizeWN.x; i++) {
        Cluster **tmp = calloc(sizeWN.y, sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "calloc()");
        new_wordlist[i] = tmp;
    }
    int index = 0;
    for (int i = 0; i < size->x; i++) {
        Size pos = find_cluster(matrixHN,sizeHN,(*wordlist)[i][0]);
        if (pos.x < lower_bd || pos.x > upper_bd)
            continue;
        for (int j = 0; j < size->y-start_index; j++)
            new_wordlist[index][j]=(*wordlist)[i][j+start_index];
        index++;
    }
    printf("New wordlist:\n");
    print_matrix(new_wordlist, sizeWN);
    *wordlist = new_wordlist;
    *size = sizeWN;
}


/* classify_clusters():
    Classify clusters into the grid or word list. If it does not fit either
    of them, it gets deleted.
*/
void classify_clusters(Cluster ****grid, Cluster ****wordlist, Line *rows,
        Line *cols, Size *sizeH, Size *sizeV, GdkPixbuf *test) {
    Cluster ***matrixH = *grid; Cluster ***matrixV = *wordlist;
    *grid = NULL;
    
    //Test in both direction to obtain the biggest result
    Cluster ***gridH, ***gridV;
    Size gridSH = find_grid(matrixH,matrixV,rows,*sizeH,*sizeV,&gridH);
    printf("Horizontal search done\n");
    Size gridSV = find_grid(matrixV,matrixH,cols,*sizeV,*sizeH,&gridV);
    if (gridSV.x>0)
        transpose_matrix(&gridV,&gridSV);

    if (gridSH.x==-1 && gridSV.x ==-1)
        printf("Grid not found. Error.\n");
    printf("TEST - horizontal(%d;%d) & vertical(%d;%d)\n",gridSH.x,gridSH.y,gridSV.x,gridSV.y);
    Size gridS;
    if (gridSV.x==-1 || gridSH.x*gridSH.y >= gridSV.x*gridSV.y) {
        gridS = gridSH;
        *grid = gridH;
    } else {
        gridS = gridSV;
        *grid = gridV;
    }
    //Marking all the clusters appart of the GRID
    for (int x = 0; x < gridS.x; x++) {
        for (int y = 0; y < gridS.y; y++)
            (*grid)[x][y]->flags |= FLAG_GRID;
    }

    printf("Grid is: (%d;%d)\n",gridS.x,gridS.y);
    print_matrix(*grid,gridS);
    //TEST RESULT
    grid_testing(*grid,gridS,test,"grid_detect.png");

    //FIND WORD LIST - ASSUME GRID HAS BEEN FOUND
    *wordlist = NULL;
    Size wordS=find_wordlist(matrixH,matrixV,rows,cols,*sizeH,*sizeV,wordlist);
    filter_wordlist(wordlist,&wordS,matrixH,*sizeH,matrixV,*sizeV);
    printf("after filter\n");
    wordList_testing(*wordlist,wordS,test,"wordlist_detect.png");

    *sizeH = gridS; *sizeV = wordS;
    //Free memory
    free(matrixH);
    free(matrixV);
}


/* cut_grid():
    Cut the grid clusters into sub-images.
*/
void cut_grid(Cluster ***grid, Size size, GdkPixbuf* input) {
    for (int x = 0; x < size.x; x++) {
        for (int y = 0; y < size.y; y++) {
            Size topL = (Size) {.x=grid[x][y]->minX,.y=grid[x][y]->minY};
            Size botR = (Size) {.x=grid[x][y]->maxX,.y=grid[x][y]->maxY};
            GdkPixbuf *letter_img = cut_from_pixbuf(input,topL,botR);
            //Calculating the filename
            char *filename;
            if (asprintf(&filename,"grid/grid_%d_%d.png",x,y) ==-1)
                errx(EXIT_FAILURE, "asprintf()");
            //Save image
            gdk_pixbuf_save(letter_img,filename,"png",NULL,NULL);
        }
    }
}


/* cut_wordlist():
    Cut the word list into sub-images.
*/
void cut_wordlist(Cluster ***wordlist, Size size, GdkPixbuf *input) {
    for (int x = 0; x < size.x; x++) {
        //Deducing the size
        int len = 0;
        for (int i = 0; i < size.y && wordlist[x][i]!=NULL;i++)
            len++;
        Size WtopL = (Size) {.x=wordlist[x][0]->minX,.y=wordlist[x][0]->minY};
        Size WbotR = (Size) {.x=wordlist[x][len-1]->maxX,
            .y=wordlist[x][len-1]->maxY};
        char *word_name;
        asprintf(&word_name,"wordlist/words_%d.png",x);
        GdkPixbuf *word_img = cut_from_pixbuf(input,WtopL,WbotR);
        gdk_pixbuf_save(word_img,word_name,"png",NULL,NULL);
        //Letters
        for (int y = 0; y < len; y++) {
            Size topL = (Size) {.x=wordlist[x][y]->minX,
                .y=wordlist[x][y]->minY};
            Size botR = (Size) {.x=wordlist[x][y]->maxX,
                .y=wordlist[x][y]->maxY};
            char *letter_name;
            asprintf(&letter_name,"wordlist/words_%d_ltr_%d.png",x,y);
            GdkPixbuf *letter_img = cut_from_pixbuf(input,topL,botR);
            gdk_pixbuf_save(letter_img,letter_name,"png",NULL,NULL);
        }
    }
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
    printf("alignement done\n");
    //STEP 4: Classify clusters
    classify_clusters(&matrixH,&matrixV,rows,cols,&sizeH,&sizeV,input);

    //STEP 5: Cut the image into sub-images
    cut_grid(matrixH,sizeH,input);
    cut_wordlist(matrixV, sizeV, input);
}

int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf);
}
