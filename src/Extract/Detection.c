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


/* expand_vertical():
    Expand the table vertically by 1
*/
void expand_vertical(Cluster** **table, Line* *row_avg, int *r_size,
        int c_size) {
    Cluster** *tmp = realloc(*table, sizeof(Cluster**)*(*r_size+1));
    Line* tmp2 = realloc(*row_avg, sizeof(Line)*(*r_size+1));
    if (!tmp || !tmp2)
        errx(EXIT_FAILURE, "realloc()");
    Cluster* *tmp4 = calloc(sizeof(Cluster*),c_size);
    if (!tmp4)
        errx(EXIT_FAILURE, "calloc()");
    tmp[*r_size] = tmp4;
    memset(&tmp2[*r_size],0,sizeof(Line));
    *table = tmp;
    *row_avg = tmp2;
    (*r_size)++;
}


/* expand_horizontal():
    Expand the table horizontally by 1
*/
void expand_horizontal(Cluster** *table, Line* *col_avg, int r_size,
        int *c_size) {
    for (int i = 0; i < r_size; i++) {
        Cluster* *tmp = realloc(table[i],sizeof(Cluster*)*(*c_size+1));
        if (!tmp)
            errx(EXIT_FAILURE, "realloc()");
        table[i] = tmp;
        table[i][*c_size] = NULL;
    }
    Line* tmp2 = realloc(*col_avg, sizeof(Line)*(*c_size+1));
    if (!tmp2)
        errx(EXIT_FAILURE, "realloc()");
    memset(&tmp2[*c_size],0,sizeof(Line));
    *col_avg = tmp2;
    (*c_size)++;
}


/* addToRow():
    Add an element to the row information.
*/
void addToRow(Line *row, Cluster ***table, int pos, int size,
        int row_nb, int c_size) {
    int total_p = 0, total_s = 0;
    //Calculate real total values
    for (int i = 0; i < c_size; i++) {
        if (table[row_nb][i]) {
            total_p+=table[row_nb][i]->centerY;
            total_s+=table[row_nb][i]->maxY-table[row_nb][i]->minY;
        }
    }
    //update
    row->nbr++;
    row->pos = total_p / row->nbr;
    row->size = total_s / row->nbr;
}


/* addToCol():
    Add an element to the col information.
*/
void addToCol(Line *col, Cluster ***table, int pos, int size,
        int col_nb, int r_size) {
    int total_p = 0, total_s = 0;
    //Calculate real total values
    for (int i = 0; i < r_size; i++) {
        if (table[i][col_nb]) {
            total_p+=table[i][col_nb]->centerX;
            total_s+=table[i][col_nb]->maxX-table[i][col_nb]->minX;
        }
    }
    //Update
    col->nbr++;
    col->pos = total_p / col->nbr;
    col->size = total_s / col->nbr;
}


/* addToTable():
    Add a cluster to the table.
*/
void addToTable(Cluster* curr, int *r_size, int *c_size,
        Cluster ****table, Line** row_avg, Line** col_avg) {
    //Data to relaunch
    Cluster **save = NULL;
    int save_s = 0;
    printf("New Cluster P(%d;%d)\n",curr->centerX,curr->centerY);
    //Add the cluster to the table
    int ROW = -1, COL = -1; //index to find
    ////Find the ROW coordinate [HORIZONTAL]
    for (int r_cmp = 0; r_cmp < *r_size; r_cmp++) {
        int TOLERANCE = (*row_avg)[r_cmp].size /2;
        printf("Check[%d]: %d(T:%d;AVG:%d)\n",r_cmp,abs(curr->centerY-(*row_avg)[r_cmp].pos),TOLERANCE,(*row_avg)[r_cmp].pos);
        if (abs(curr->centerY - (*row_avg)[r_cmp].pos) <= TOLERANCE) {
            //Add it to this horizontal line
            ROW = r_cmp;
            break;
        }
    }
    printf("Row:%d\n",ROW);
    if (ROW == -1) { //Did not find: create a new line after
        if (*r_size == 1) {
            ROW = 1; //Just create the second line
            expand_vertical(table, row_avg, r_size,*c_size);
        } else {
            int median_hspace = 0, counth = 0;
            for (int i = 1; i < *r_size; i++) {
                median_hspace+=(*row_avg)[i].pos-(*row_avg)[i-1].pos;
                counth++;
            }
            median_hspace/=counth;
            printf("median: %d\n",median_hspace);
            int toexpand=(curr->centerY-(*row_avg)[*r_size-1].pos)/
                median_hspace;
            if (toexpand == 0)
                toexpand++;
            printf("toexpand:%d\n",toexpand);
            for (int i = 0; i < toexpand; i++)
                expand_vertical(table,row_avg,r_size,*c_size);
            ROW = *r_size - 1;
        }
    }
    ////Find the COL coordinate [VERTICAL]
    for (int c_cmp = 0; c_cmp < *c_size; c_cmp++) {
        int TOLERANCE = (*col_avg)[c_cmp].size / 2;
        printf("Check[%d]: %d(T:%d;AVG:%d)\n",c_cmp,abs(curr->centerX-(*col_avg)[c_cmp].pos),TOLERANCE,(*col_avg)[c_cmp].pos);
        if (abs(curr->centerX - (*col_avg)[c_cmp].pos) <= TOLERANCE) {
            //Add it to this vertical line
            COL = c_cmp;
            break;
        } else if (curr->centerX < (*col_avg)[c_cmp].pos) {
            //saving them
            save = malloc(sizeof(Cluster*)*(*r_size));
            save_s = *r_size;
            if(!save)
                errx(EXIT_FAILURE,"malloc()");
            for (int x = 0; x < *r_size; x++) {
                save[0] = (*table)[x][c_cmp];
                (*table)[x][c_cmp] = NULL;
            }
            //update values
            COL = c_cmp;
            break;
        }
    }
    printf("Col:%d\n",COL);
    if (COL == -1) {
        if (*c_size == 1) {
            COL = 1; //Just create the second col
            expand_horizontal(*table,col_avg,*r_size,c_size);
        } else {
            if (COL == -1) {
                int median_vspace = 0, countv = 0;
                for (int i = 1; i < *c_size; i++) {
                    median_vspace+=(*col_avg)[i].pos-(*col_avg)[i-1].pos;
                    countv++;
                }
                median_vspace/=countv;
                int toexpand=(curr->centerX-(*col_avg)[*c_size-1].pos)/
                    median_vspace;
                if (toexpand==0)
                    toexpand++;
                printf("toexpand:%d\n",toexpand);
                for (int i = 0; i < toexpand; i++)
                    expand_horizontal(*table,col_avg,*r_size, c_size);
                COL = *c_size - 1;
            }
        }
    }
    //Insert Cluster into table & Update variables
    printf("Insert at: %d;%d\n\n",ROW,COL);
    (*table)[ROW][COL] = curr;
    addToRow(&((*row_avg)[ROW]),*table,curr->centerY,curr->maxY-curr->minY,
            ROW,*c_size);
    addToCol(&((*col_avg)[COL]),*table,curr->centerX,curr->maxX-curr->minX,
            COL,*r_size);
    for (int i = 0; i < save_s; i++)
        addToTable(save[i],r_size,c_size,table,row_avg,col_avg);
}


/* classify_clusters():
    Categorize clusters in two categories: grid or word list.
    If a cluster is not appart of them two, it is deleted.
*/
Cluster** *classify_clusters(Cluster **clusters, int *rs, int *cs,
        Line* *rows, Line* *cols) {
    //STEP 1: INITIALIZATION
    //Create the SPACIAL CLUSTERING TABLE
    int r_size = 1, c_size = 1;
    printf("Size:%d;%d\n",r_size,c_size);
    Cluster ***table = malloc(sizeof(Cluster**));
    if (!table)
        errx(EXIT_FAILURE,"malloc()");
    table[0] = malloc(sizeof(Cluster*));
    if (!table[0])
        errx(EXIT_FAILURE,"malloc()");
    //Put the first cluser - assume non-empty
    table[0][0] = *clusters;
    //Creating the rows and cols average positions
    Line* row_avg = malloc(sizeof(Line));
    Line* col_avg = malloc(sizeof(Line));
    if (!row_avg || !col_avg)
        errx(EXIT_FAILURE,"malloc()");
    row_avg[0] = (Line) {
        .pos=(*clusters)->centerY,
        .size=(*clusters)->maxY-(*clusters)->minY,
        .nbr=1
    };
    col_avg[0] = (Line) {
        .pos=(*clusters)->centerX,
        .size=(*clusters)->maxX-(*clusters)->minX,
        .nbr=1
    };

    //STEP 2: Iteration of clusters
    printf("Iteration\n");
    Cluster *curr = (*clusters)->next;
    while (curr) {
        addToTable(curr,&r_size,&c_size,&table,&row_avg,&col_avg);
        curr = curr->next;
    }
    *rs = r_size;
    *cs = c_size;
    *rows = row_avg;
    *cols = col_avg;
    return table;
}


//JUST DO TEST - //TODO: to REMOVE
void print_testing(Cluster ***table, int xs, int ys, Line* rows, Line* cols) {
    printf("PRINT MATRIX:\n");
    for (int y = 0; y < ys; y++)
        printf("%d ",cols[y].pos);
    printf("\n");
    for (int y = 0; y < ys; y++)
        printf("%d ",cols[y].size);
    printf("\n");
    for (int x = 0; x < xs; x++) {
        printf("%d %d ",rows[x].pos,rows[x].size);
        for (int y = 0; y < ys; y++)
            printf("%c ",table[x][y] == NULL ? '\\':'X');
        printf("\n");
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
    testing(start,count,input,"iterate.png");
    //STEP 2: Filter clusters
    count = threshold_filter(&start, count, median_size);
    testing(start,count,input,"thresholdfilter.png");

    //STEP 3: Classify clusters
    int xs,ys;
    Line* rows,*cols;
    Cluster ***table = classify_clusters(&start,&xs,&ys,&rows,&cols);
    printf("With %d;%d\n",xs,ys);
    print_testing(table,xs,ys,rows,cols);
}

int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf);
}
