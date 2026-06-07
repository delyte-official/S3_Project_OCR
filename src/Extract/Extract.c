#define _GNU_SOURCE
#include <err.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include "Extract.h"
#define THRESHOLD_MIN 10


/* free_matrix():
    Free a dynamically allocated matrix but not its content.
*/
void free_matrix(Cluster ***matrix, int xs) {
    for (int x = 0; x < xs; x++)
        free(matrix[x]);
    free(matrix);
}


/* free_cluster():
    Free the given cluster.
*/
void free_cluster(Cluster *to_free) {
    Pixel *curr = to_free->pixels;
    while (curr!=NULL) {
        Pixel *next = curr->next;
        free(curr);
        curr = next;
    }
    free(to_free);
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
Pixel *dfs(Cluster *end, Pixel *old, int x, int y, gboolean** visit,
        GdkPixbuf *pixbuf) {
    end->size++;
    Pixel *new_pixel = malloc(sizeof(Pixel));
    if (!new_pixel)
        errx(EXIT_FAILURE, "malloc()");
    new_pixel->x = x;
    new_pixel->y = y;
    new_pixel->next = NULL;
    old->next = new_pixel;
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
    int dx[] = {-1,0,0,1};
    int dy[] = {0,-1,1,0};
    for (int i = 0; i < 4; i++) { //Calculating new coordinates
        int new_x =x+dx[i];
        int new_y = y+dy[i];
        if (new_x < 0 || new_y < 0 || new_y >= height ||new_x >= width)
            continue;
        guchar *p = pixels + new_x * N + new_y*rowstride;
        if (visit[new_x][new_y] || !is_black(p))
            continue;
        new_pixel =dfs(end,new_pixel, new_x, new_y, visit, pixbuf);
    }
    return new_pixel;
}


/* initCluster():
    Initialize a new cluster at the given pointer.
*/
Cluster* initCluster(int x, int y, gboolean** visit,
        GdkPixbuf *pixbuf) {
    Cluster *end = malloc(sizeof(Cluster));
    *end = (Cluster) {
        .size = 0,
        .centerX = 0, .centerY = 0,
        .maxX = x, .minX = x,
        .maxY = y, .minY = y,
        .pixels = NULL,
        .flags = 0
    };
    Pixel *pixel = malloc(sizeof(Pixel));
    if (!pixel)
        errx(EXIT_FAILURE, "malloc()");
    pixel->x = x;
    pixel->y = y;
    pixel->next = NULL;
    end->pixels = pixel;
    dfs(end, pixel, x, y, visit, pixbuf);
    end->centerX = (end->minX + end->maxX)/2;
    end->centerY = (end->minY + end->maxY)/2;
    if (end->size < THRESHOLD_MIN)
        return NULL;
    return end;
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
    Cluster *end = NULL;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //new cluster?
            guchar *pixel = pixels + y * rowstride + x * N;
            if (is_black(pixel) && !visited[x][y]) {//0 means a black pixel
                Cluster *new_C = initCluster(x, y, visited, input);
                if (new_C==NULL)
                    continue;
                if (start == NULL) {
                    start = new_C;
                    end = start;
                } else {
                    end->next = new_C;
                    end = end->next;
                }
                *median+=new_C->size;
                clusterCount++;
            }
        }
    }
    //Free data
    for (int i = 0; i < width; i++)
        free(visited[i]);
    free(visited);
    //Set data
    *last = start;
    *median/=clusterCount;
    return clusterCount;
}


/* threshold_filer():
    Filter all clusters according to a threshold.
*/
int threshold_filter(Cluster **clusters, int count, int median) {
    const int threshold = 0.9f *median;
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
            free_cluster(curr);
            curr = next;
            count--;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
    return count;
}


/* expand_cols():
    Expand the table on y by 1
*/
void expand_cols(Cluster** *table, int xs, int *ys, int size) {
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


/* expand_rows():
    Expand the table on x by 1.
*/
void expand_rows(Cluster** **table, int *xs, int ys, int size) {
    if (ys==0)
        ys++;
    int difference = size-*xs;
    if (difference < 0) {
        for (int i = *xs-1; i > size-1; i--)
            free((*table)[i]);
    }
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
            expand_rows(&rows, &rows_x,rows_y, rows_x+1);
            expand_line(&row_avg,rows_x);
            ROW = rows_x - 1;
        }
        //Detect if line is full
        if (rows_y < 1 || rows[ROW][rows_y-1] != NULL)
            expand_cols(rows,rows_x,&rows_y,rows_y+1);
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
            expand_rows(&cols, &cols_x,cols_y,cols_x+1);
            expand_line(&col_avg,cols_x);
            COL = cols_x - 1;
        }
        //Detect if line is full
        if (cols_y < 1 || cols[COL][cols_y-1] != NULL)
            expand_cols(cols,cols_x,&cols_y,cols_y+1);
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

    *matrixH = rows;
    *matrixV = cols;
    *rows_p = row_avg;
    *cols_p = col_avg;
    *sizeH = (Size) {.rows=rows_x,.cols=rows_y};
    *sizeV = (Size) {.rows=cols_x,.cols=cols_y};
    //Freeing
    free(sortHori);
    free(sortVert);
}


/* find_cluster():
    Returns the coordinates of a cluster in a cluster matrix.
*/
Position find_cluster(Cluster ***matrix, Size size, Cluster* tofind) {
    for (int x = 0; x < size.rows; x++) {
        for (int y = 0; y < size.cols; y++) {
            if (matrix[x][y]==NULL)
                break; //Skip line
            if (matrix[x][y]==tofind)
                return (Position) {.x=x,.y=y};
        }
    }
    return (Position) {.x=-1,.y=-1};
}


/* check_align():
    Check if two clusters are align in the SRC matrix.
*/
int check_align(Cluster ***SRC, Cluster *first, Cluster *second,
        Size size) {
    //Find the x coordinates of the two
    Position pos1 = find_cluster(SRC,size,first);
    Position pos2 = find_cluster(SRC,size,second);
    return pos1.x == pos2.x && pos1.x != -1;
}


/* find_grid():
    Tries to find the grid in the SRC matrix with help of CMP matrix.
*/
Size find_grid(Cluster ***SRC, Cluster ***CMP, Size sizeSRC, Size sizeCMP,
        Cluster ****DST) {
    Size sizeG = (Size) {.rows=0,.cols=0};
    *DST = NULL;
    ////FIND GRID
    //Search for grid in matrix HORIZONTAL
    for (int x = 0; x < sizeSRC.rows; x++) {
        for (int y = 0; y < sizeSRC.cols; y++) {
            if (SRC[x][y]==NULL)
                break; //row empty from now on
            sizeG.rows = 0; sizeG.cols = 0;
            //Begin finding for Cluster at (x;y)
            for (int i = y; i < sizeSRC.cols && SRC[x][i]!=NULL; i++)
                sizeG.rows++;
            if (sizeG.rows < 5)
                break; //Skip line
            int expand = sizeG.rows; sizeG.rows = 0; sizeG.cols = 1;
            expand_rows(DST,&sizeG.rows,sizeG.cols,expand);
            //Putting the first line into the grid matrix
            for (int i = 0; i < sizeG.rows; i++)
                (*DST)[i][0]=SRC[x][y+i];
            for (int j = x+1; j < sizeSRC.rows; j++) {
                int line_c = 0;
                int start_line = -1;
                for (int i = 0; i <= sizeSRC.cols-5; i++) {
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
                for (int i = start_line; i-start_line<sizeG.rows &&
                        i<sizeSRC.cols; i++) {
                    if (SRC[j][i] == NULL || !check_align(CMP,
                                SRC[x][y+i-start_line],SRC[j][i],sizeCMP)) {
                        if (line_c < sizeG.rows && line_c>=5)
                            expand_rows(DST,&sizeG.rows,sizeG.cols,line_c);
                        break;
                    }
                    line_c++;
                }
                if (line_c < 5)
                    break;
                expand_cols(*DST,sizeG.rows,&sizeG.cols,sizeG.cols+1);
                for (int i = 0; i < sizeG.rows; i++)
                    (*DST)[i][sizeG.cols-1]=SRC[j][start_line+i];
            }
            if (sizeG.cols >= 5)
                break;
            //Reset TRIAL
            free_matrix(*DST,sizeG.rows);
            sizeG.rows = 0;
            sizeG.cols = 0;
            *DST=NULL;
        }
        if (sizeG.cols >= 5)
            break;
    }
    if (sizeG.rows < 1 || sizeG.cols < 5)
        return (Size) {.rows=-1,.cols=-1};
    return sizeG;
}


/* find_wordlist():
    Detects the word list given two alignment matrices.
*/
Size find_wordlist(Cluster ***matrixH, Cluster***matrixV, Size sizeH,
        Size sizeV, Cluster ****wordList) {
    Size sizeW = (Size) {.rows=0,.cols=0};
    //Iteratove over the vertical matrix to find unflagged clusters
    for (int x = 0; x < sizeV.rows; x++) {
        for (int y = 0; y < sizeV.cols; y++) {
            if (matrixV[x][y]==NULL)
                break; //Skip column
            if (matrixV[x][y]->flags)
                continue; //Flagged => skip
            //Find its position in the horizontal matrix
            Position pos = find_cluster(matrixH,sizeH,matrixV[x][y]);
            //Grab the longest word from there
            int len = 1;
            for (int i = pos.y+1; i < sizeH.cols &&
                    matrixH[pos.x][i]!=NULL;i++){
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
            expand_rows(wordList,&sizeW.rows,sizeW.cols,sizeW.rows+1);
            if (len+1>sizeW.cols) //+1 for NULL terminator
                expand_cols(*wordList,sizeW.rows,&sizeW.cols,len+1);
            //Save the values and flag the clusters
            for (int i = 0; i < len; i++) {
                (*wordList)[sizeW.rows-1][i]=matrixH[pos.x][pos.y+i];
                matrixH[pos.x][pos.y+i]->flags |= FLAG_WORD;
            }
            (*wordList)[sizeW.rows-1][len]=NULL; //Null terminator
        }
    }
    return sizeW;
}


/* transpose_matrix():
    Transpose a matrix, X*Y => Y*X.
*/
void transpose_matrix(Cluster ****matrix, Size *size) {
    Cluster ***transposed = malloc(size->cols*sizeof(Cluster**));
    if (!transposed)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < size->cols; i++) {
        Cluster **tmp = malloc(size->rows*sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "malloc()");
        transposed[i] = tmp;
    }

    //Transpose data
    for (int i = 0; i < size->rows; i++) {
        for (int j = 0; j < size->cols; j++) {
            transposed[j][i] = (*matrix)[i][j];
        }
        free((*matrix)[i]);
    }
    free(*matrix);
    *matrix = transposed;
    *size = (Size) {.rows=size->cols,.cols=size->rows};
}


/* filter_wordlist():
    Remove the groups of clusters that have been considered "words" but
    are not appart of the word list.
*/
void filter_wordlist(Cluster ****wordlist, Size *size, Cluster ***matrixH,
        Size sizeH, Cluster ***matrixV, Size sizeV) {
    ////Build alignement matrices
    int *calcH = calloc(sizeH.rows,sizeof(int));
    int *calcV = calloc(sizeV.rows,sizeof(int));
    if (!calcH || !calcV)
        errx(EXIT_FAILURE, "calloc()");
    for (int i = 0; i < size->rows; i++) {
        //Horizontal alignment
        Position pos = find_cluster(matrixH,sizeH,(*wordlist)[i][0]);
        calcH[pos.x]++;
        //Vertical alignment
        pos = find_cluster(matrixV,sizeV,(*wordlist)[i][0]);
        calcV[pos.x]++;
    }
    Size sizeHN = (Size) {.rows=0,.cols=0};
    for (int i = 0; i < sizeH.rows; i++) {
        if (calcH[i] == 0)
            continue;
        sizeHN.rows++;
        if (calcH[i]>sizeHN.cols)
            sizeHN.cols=calcH[i];
    }
    Size sizeVN = (Size) {.rows=0,.cols=0};
    for (int i = 0; i < sizeV.rows; i++) {
        if (calcV[i]==0)
            continue;
        sizeVN.rows++;
        if (calcV[i]>sizeVN.cols)
            sizeVN.cols=calcV[i];
    }
    
    //Allocate matrices
    Cluster ***matrixHN = malloc(sizeof(Cluster**)*sizeHN.rows);
    if (!matrixHN)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < sizeHN.rows; i++) {
        Cluster **tmp = calloc(sizeHN.cols,sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "calloc()");
        matrixHN[i]=tmp;
    }

    //Fill matrices
    for (int i = 0; i < size->rows; i++) {
        //Horizontal alignment
        Position pos = find_cluster(matrixH,sizeH,(*wordlist)[i][0]);
        Position index = (Position) {.x=0,.y=0};
        for (int j = 0; j < pos.x; j++) {
            if (calcH[j]!=0)
                index.x++;
        }
        for (int j = 0; j < sizeHN.cols && matrixHN[index.x][j]!=NULL; j++)
            index.y++;
        matrixHN[index.x][index.y] = (*wordlist)[i][0];
    }
    free(calcH);
    free(calcV);

    //NEXT STEP: filter out those too far in both matrices
    int upper_bd = -1, min = INT_MAX;
    for (int i = 1; i < sizeHN.rows; i++) {
        int distance = matrixHN[i][0]->minY-matrixHN[i-1][0]->maxY;
        if (distance < min) {
            min = distance;
            upper_bd = i;
        }
    }
    int TOLERANCE = min * 1.2f;
    int lower_bd = upper_bd-1;
    for (int i = lower_bd; i > 0; i--, lower_bd--) {
        int distance = matrixHN[i][0]->minY-matrixHN[i-1][0]->maxY;
        if (distance > TOLERANCE)
            break;
    }
    for (int i = upper_bd; i < sizeHN.rows-1; i++, upper_bd++) {
        int distance = matrixHN[i+1][0]->minY-matrixHN[i][0]->maxY;
        if (distance > TOLERANCE)
            break;
    }
    //Rebuild wordlist
    Size sizeWN = (Size) {.rows=0,.cols=0};
    int count_bullets = 0;
    for (int i = 0; i < size->rows; i++) {
        Cluster *curr = (*wordlist)[i][0];
        Position pos = find_cluster(matrixHN,sizeHN,curr);
        if (pos.x > upper_bd || pos.x < lower_bd)
            continue;
        float density = (float)(curr->size)/
            ((curr->maxX-curr->minX+1)*(curr->maxY-curr->minY+1));
        if (density < 0.2f) //threshold between a fake and real letter
            count_bullets++;
        sizeWN.rows++;
        int count = 0;
        for (int j = 0; j < size->cols && (*wordlist)[i][j]!=NULL; j++)
            count++;
        if (count > sizeWN.cols)
            sizeWN.cols = count;
    }
    int start_index = 0;
    sizeWN.cols++; //Taking into account NULL pointer
    Cluster ***new_wordlist = malloc(sizeof(Cluster**)*sizeWN.rows);
    if (!new_wordlist)
        errx(EXIT_FAILURE, "malloc()");
    for (int i = 0; i < sizeWN.rows; i++) {
        Cluster **tmp = calloc(sizeWN.cols, sizeof(Cluster*));
        if (!tmp)
            errx(EXIT_FAILURE, "calloc()");
        new_wordlist[i] = tmp;
    }
    int index = 0;
    for (int i = 0; i < size->rows; i++) {
        Position pos = find_cluster(matrixHN,sizeHN,(*wordlist)[i][0]);
        if (pos.x < lower_bd || pos.x > upper_bd)
            continue;
        for (int j = 0; j < size->cols-start_index; j++)
            new_wordlist[index][j]=(*wordlist)[i][j+start_index];
        index++;
    }
    //Freeing old wordlist
    free_matrix(*wordlist,size->rows);
    free_matrix(matrixHN,sizeHN.rows);
    *wordlist = new_wordlist;
    *size = sizeWN;
}


/* classify_clusters():
    Classify clusters into the grid or word list. If it does not fit either
    of them, it gets deleted.
*/
int classify_clusters(Cluster ****grid, Cluster ****wordlist, Size *sizeH,
        Size *sizeV) {
    Cluster ***matrixH = *grid; Cluster ***matrixV = *wordlist;
    *grid = NULL; *wordlist = NULL;

    //Test in both direction to obtain the biggest result
    Cluster ***gridH, ***gridV;
    Size gridSH=find_grid(matrixH,matrixV,*sizeH,*sizeV,&gridH);
    Size gridSV=find_grid(matrixV,matrixH,*sizeV,*sizeH,&gridV);
    if (gridSV.rows>0)
        transpose_matrix(&gridV,&gridSV);

    if (gridSH.rows==-1 && gridSV.rows ==-1) {
        g_log("App",G_LOG_LEVEL_ERROR,"Could not find the grid. Manual"
                " input required.");
        return -1;
    }
    Size gridS;
    if (gridSV.rows==-1 || gridSH.rows*gridSH.cols >= gridSV.rows*gridSV.cols) {
        gridS = gridSH;
        *grid = gridH;
        free_matrix(gridV,gridSV.rows);
    } else {
        gridS = gridSV;
        *grid = gridV;
        free_matrix(gridH,gridSH.rows);
    }
    //Marking all the clusters appart of the GRID
    for (int x = 0; x < gridS.rows; x++) {
        for (int y = 0; y < gridS.cols; y++)
            (*grid)[x][y]->flags |= FLAG_GRID;
    }

    //FIND WORD LIST - ASSUME GRID HAS BEEN FOUND
    Size wordS=find_wordlist(matrixH,matrixV,*sizeH,*sizeV,wordlist);
    if (wordS.rows == 0 || wordS.cols==0) {
        g_log("App",G_LOG_LEVEL_ERROR,"Could not find the wordlist. Manual"
                " input required.");
        return -1;
    }
    filter_wordlist(wordlist,&wordS,matrixH,*sizeH,matrixV,*sizeV);
    //Free memory
    free_matrix(matrixH,sizeH->rows);
    free_matrix(matrixV,sizeV->rows);
    //Set data
    *sizeH = gridS; *sizeV = wordS;
    return 1;
}


/* cut_from_pixbuf():
    Cut a pixbuf into a sub-pixbuf given two coordinates of a rectangle.
*/
GdkPixbuf* cut_from_pixbuf(GdkPixbuf *original, Position topL, Position botR)
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


/* save_cluster_pixbuf():
    Save a cluster as a pixbuf.
*/
GdkPixbuf *save_cluster_pixbuf(Cluster *input) {
    int width = input->maxX-input->minX+1;
    int height = input->maxY-input->minY+1;
    GdkPixbuf *res = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, width,
            height);
    int rowstride = gdk_pixbuf_get_rowstride(res);
    int n = gdk_pixbuf_get_n_channels(res);
    guchar *pixels = gdk_pixbuf_get_pixels(res);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            guchar *p = pixels + y*rowstride +x*n;
            p[0]=p[1]=p[2]=255;
        }
    }
    //Redraw the pixels
    Pixel *curr = input->pixels;
    while (curr != NULL) {
        int x = curr->x-input->minX;
        int y = curr->y-input->minY;
        guchar *p = pixels + y*rowstride +x*n;
        p[0]=p[1]=p[2]=0;
        curr = curr->next;
    }
    return res;
}


/* cut_grid():
    Cut the grid clusters into sub-images.
*/
void cut_grid(Cluster ***grid, Size size, char *output) {
    for (int x = 0; x < size.rows; x++) {
        for (int y = 0; y < size.cols; y++) {
            GdkPixbuf *letter_img = save_cluster_pixbuf(grid[x][y]);
            //Calculating the filename
            char *filename;
            if (asprintf(&filename,"%sgrid_%d_%d.png",output,x,y) ==-1)
                errx(EXIT_FAILURE, "asprintf()");
            //Save image
            gdk_pixbuf_save(letter_img,filename,"png",NULL,NULL);
            g_object_unref(letter_img);
            free(filename);
        }
    }
}


/* cut_wordlist():
    Cut the word list into sub-images.
*/
void cut_wordlist(Cluster ***wordlist, Size size, GdkPixbuf *input,
        char *output) {
    for (int x = 0; x < size.rows; x++) {
        //Deducing the size
        int len = 0;
        for (int i = 0; i < size.cols && wordlist[x][i]!=NULL;i++)
            len++;
        Position WtopL = (Position) {.x=wordlist[x][0]->minX,
            .y=wordlist[x][0]->minY};
        Position WbotR = (Position) {.x=wordlist[x][len-1]->maxX,
            .y=wordlist[x][len-1]->maxY};
        char *word_name;
        asprintf(&word_name,"%swords_%d.png",output,x);
        GdkPixbuf *word_img = cut_from_pixbuf(input,WtopL,WbotR);
        gdk_pixbuf_save(word_img,word_name,"png",NULL,NULL);
        free(word_name);
        g_object_unref(word_img);
        //Letters
        for (int y = 0; y < len; y++) {
            char *letter_name;
            asprintf(&letter_name,"%swords_%d_ltr_%d.png",output,x,y);
            GdkPixbuf *letter_img = save_cluster_pixbuf(wordlist[x][y]);
            gdk_pixbuf_save(letter_img,letter_name,"png",NULL,NULL);
            free(letter_name);
            g_object_unref(letter_img);
        }
    }
}
