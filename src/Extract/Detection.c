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
void expand_vertical(Cluster** **table, int* *row_avgP, int* *row_avgS,
        int *r_size, int c_size) {
    Cluster** *tmp = realloc(*table, sizeof(Cluster**)*(*r_size+1));
    int* tmp2 = realloc(*row_avgP, sizeof(int)*(*r_size+1));
    int* tmp3 = realloc(*row_avgS, sizeof(int)*(*r_size+1));
    if (!tmp || !tmp2 || !tmp3)
        errx(EXIT_FAILURE, "realloc()");
    Cluster* *tmp4 = calloc(sizeof(Cluster*),c_size);
    if (!tmp4)
        errx(EXIT_FAILURE, "calloc()");
    tmp[*r_size] = tmp4;tmp2[*r_size]=0;tmp3[*r_size]=0;
    *table = tmp;
    *row_avgP = tmp2;
    *row_avgS = tmp3;
    *r_size++;
}


/* expand_horizontal():
    Expand the table horizontally by 1
*/
void expand_horizontal(Cluster** *table, int* *col_avgP, int* *col_avgS,
        int r_size, int *c_size) {
    for (int i = 0; i < r_size; i++) {
        Cluster* *tmp = realloc(table[i],sizeof(Cluster*)*(*c_size+1));
        if (!tmp)
            errx(EXIT_FAILURE, "realloc()");
        table[i] = tmp;
        table[i][*c_size] = NULL;
    }
    int* tmp2 = realloc(*col_avgP, sizeof(int)*(*c_size+1));
    int* tmp3 = realloc(*col_avgS, sizeof(int)*(*c_size+1));
    if (!tmp2 || !tmp3)
        errx(EXIT_FAILURE, "realloc()");
    *col_avgP = tmp2;
    *col_avgS = tmp3;
    *c_size++;
}


/* classify_clusters():
    Categorize clusters in two categories: grid or word list.
    If a cluster is not appart of them two, it is deleted.
*/
Cluster** *classify_clusters(Cluster **clusters, int *rs, int *cs) {
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
    int* row_avgP = malloc(sizeof(int));
    int* row_avgS = malloc(sizeof(int));
    int* col_avgP = malloc(sizeof(int));
    int* col_avgS = malloc(sizeof(int));
    if (!row_avgP || !col_avgP || !row_avgS || !col_avgS)
        errx(EXIT_FAILURE,"malloc()");
    row_avgP[0] = (*clusters)->centerX;
    row_avgS[0] = (*clusters)->maxY-(*clusters)->minY;
    col_avgP[0] = (*clusters)->centerY;
    col_avgS[0] = (*clusters)->maxX-(*clusters)->minX;

    //STEP 2: Iteration of clusters
    printf("Iteration\n");
    Cluster *curr = (*clusters)->next;
    while (curr) {
        printf("New Cluster P(%d;%d)\n",curr->centerX,curr->centerY);
        //Add the cluster to the table
        int ROW = -1, COL = -1; //index to find
        ////Find the ROW coordinate [HORIZONTAL]
        for (int r_cmp = 0; r_cmp < r_size; r_cmp++) {
            int TOLERANCE = row_avgS[r_cmp] /2;
            printf("Check[%d]: %d(T:%d;AVG:%d)\n",r_cmp,abs(curr->centerY-row_avgP[r_cmp]),TOLERANCE,row_avgP[r_cmp]);
            if (abs(curr->centerY - row_avgP[r_cmp]) < TOLERANCE) {
                //Add it to this horizontal line
                ROW = r_cmp;
                break;
            }
        }
        printf("Row:%d\n",ROW);
        if (ROW == -1) { //Did not find: create a new line after
            if (r_size == 1) {
                ROW = 1; //Just create the second line
                expand_vertical(&table, &row_avgP, &row_avgS, &r_size,c_size);
            } else {
                int median_hspace, counth = 0;
                for (int i = 1; i < r_size; i++) {
                    median_hspace+=row_avgP[i]-row_avgP[i-1];
                    counth++;
                }
                median_hspace/=counth;
                int toexpand=(curr->centerX-row_avgP[r_size-1])/median_hspace;
                if (toexpand == 0)
                    toexpand++;
                for (int i = 0; i < toexpand; i++)
                    expand_vertical(&table,&row_avgP,&row_avgS,&r_size,c_size);
                ROW = r_size - 1;
            }
        }
        ////Find the COL coordinate [VERTICAL]
        for (int c_cmp = 0; c_cmp < c_size; c_cmp++) {
            int TOLERANCE = col_avgS[c_cmp] / 2;
            if (abs(curr->centerX - col_avgP[c_cmp]) < TOLERANCE) {
                //Add it to this vertical line
                COL = c_cmp;
                break;
            }
        }
        if (COL == -1) {
            if (c_size == 1) {
                COL = 1; //Just create the second col
                expand_horizontal(table,&col_avgP,&col_avgS,r_size,&c_size);
            } else {
                if (COL == -1) {
                    int median_vspace, countv = 0;
                    for (int i = 1; i < c_size; i++) {
                        median_vspace+=col_avgP[i]-col_avgP[i-1];
                        countv++;
                    }
                    median_vspace/=countv;
                    int toexpand=(curr->centerY-col_avgP[c_size-1])/
                        median_vspace;
                    if (toexpand==0)
                        toexpand++;
                    for (int i = 0; i < toexpand; i++)
                        expand_horizontal(table,&col_avgP,&col_avgS,r_size,
                                &c_size);
                    ROW = c_size - 1;
                }
            }
        }
        //Insert Cluster into table
        printf("Insert at: %d;%d\n",ROW,COL);
        table[ROW][COL] = curr;
        curr = curr->next;
    }
    *rs = r_size;
    *cs = c_size;
    return table;
}


//JUST DO TEST - //TODO: to REMOVE
void print_testing(Cluster ***table, int xs, int ys) {
    printf("PRINT MATRIX:\n");
    for (int x = 0; x < xs; x++) {
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
    Cluster ***table = classify_clusters(&start,&xs,&ys);
    printf("With %d;%d\n",xs,ys);
    print_testing(table,xs,ys);
}

int main(int argc, char* argv[]) {
    if (argc!=2)
        return 1;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(argv[1], NULL);
    extract_information(pixbuf);
}
