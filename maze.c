#include<stdio.h>
#include<stdlib.h>
#define chars 12
#define q_size 10
#define INT_MAX 200
// #define DEBUG

struct Node{
    int vertex;
    struct Node *next;
};

struct Graph{
    int vertices;
    struct Node *adList;
};

struct Queue{
    int *arr;
    int front, rear, size;
};

struct Node *createNode(int v){
    struct Node *n = (struct Node *)malloc(sizeof(struct Node));
    n->vertex = v;
    n->next = NULL;
    return n;
}

struct Graph *createGraph(int v){
    struct Graph *g = (struct Graph *)malloc(sizeof(struct Graph));
    g->vertices = v;
    g->adList = (struct Node *)malloc(v * sizeof(struct Node));
    for(int i = 0; i < v; i++){
        g->adList[i].vertex = i;
        g->adList[i].next = NULL;
    }
    return g;
}

void add_edge(struct Graph *g, int a, int b){
    struct Node *temp = &(g->adList[a]);
    while(temp->next != NULL)
        temp = temp->next;
    temp->next = createNode(b);
}

struct Queue * createQueue(int size){
    struct Queue *queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->arr = (int*)malloc(size * sizeof(int));
    queue->front = -1;
    queue->rear = -1;
    queue->size = size;
    return queue;
}

int isQueueEmpty(struct Queue *queue){
    if(queue->front == -1 && queue->rear == -1) return 1;
    return 0;
}

int isQueueFull(struct Queue *queue){
    if(queue->rear < queue->front && queue->rear + 1 == queue->front)  return 1;
    else if(queue->rear - queue->front == queue->size - 1) return 1;
    return 0;
}

int dequeue(struct Queue *queue){
    int ele = *(queue->arr + queue->front);
    if(queue->front == queue->rear){
        queue->front = -1;
        queue->rear = -1;
    }
    else
        queue->front = (queue->front + 1) % queue->size;
    return ele;
}

struct Queue * increaseQueue(struct Queue *queue){
    int *tempArr = (int *)malloc(2 * queue->size * sizeof(int));
    char isempty;
    #ifdef DEBUG
        printf("Copying ....");
    #endif
    for(int i = 0; i < queue->size; i++){
        tempArr[i] = dequeue(queue);
    }
    #ifdef DEBUG
        printf("Copied.\n");
    #endif
    free(queue->arr);
    queue->arr = tempArr;
    tempArr = NULL;
    queue->front = 0;
    queue->rear = queue->size - 1;
    queue->size = 2 * queue->size;
    return queue;
}

void enqueue(struct Queue *queue, int ele){
    if(isQueueFull(queue)){
        queue = increaseQueue(queue);
    }
    queue->rear = (queue->rear + 1) % queue->size;
    *(queue->arr + queue->rear) = ele;
    if(queue->front == -1)
        queue->front = 0;
}

char **read_file(FILE *fin){
    char **str = (char **)malloc(chars * sizeof(char *));
    for(int i = 0; !feof(fin) && i < chars; i++){
        str[i] = (char *)malloc(chars * sizeof(char));
        fscanf(fin, "%s", str[i]);
        fgetc(fin);
    }
    return str;
}

int get_start_position(char **s_maze){
    for(int i = 0; i < chars; i++){
        if(s_maze[i][0] == '.')
            return i * chars;
    }
    return -1;
}

struct Graph *construct_graph(char **maze){
    struct Graph *graph = createGraph(chars * chars);
    int vertex;
    for(int i = 0; i < chars; i++){
        for(int j = 0; j < chars; j++){
            if(maze[i][j] == '.'){
                vertex = (i * chars) + j;
                //left
                if(j != 0 && maze[i][j-1] == '.'){
                    add_edge(graph, vertex, vertex - 1);
                }
                //up
                if(i != 0 && maze[i-1][j] == '.'){
                    add_edge(graph, vertex, vertex - chars);
                }
                //right
                if(j != chars-1 && maze[i][j+1] == '.'){
                    add_edge(graph, vertex, vertex + 1);
                }
                //down
                if(i != chars-1 && maze[i+1][j] == '.'){
                    add_edge(graph, vertex, vertex + chars);
                }
            }
        }
    }
    return graph;
}

int get_min(int a, int b){
    if(a <= b)  return a;
    return b;
}

int get_path_distance(struct Graph *graph, int src){
    struct Node *temp;
    int visited[graph->vertices], distance[graph->vertices], v, dest = -1;
    for(int i = 0; i < graph->vertices; i++){
        visited[i] = 0;
        distance[i] = INT_MAX;
    }
    struct Queue *queue = createQueue(q_size);
    enqueue(queue, src);
    visited[src] = 1;
    distance[src] = 0;
    while(!isQueueEmpty(queue)){
        v = dequeue(queue);
        if((v % chars) == (chars - 1)){
            dest = v;
        }
        temp = graph->adList[v].next;
        while(temp != NULL){
            if(visited[temp->vertex] == 0){
                visited[temp->vertex] = 1;
                distance[temp->vertex] = get_min(distance[v] + 1, distance[temp->vertex]);
                enqueue(queue, temp->vertex);
            }
            temp = temp->next;
        }
    }
    if(dest == -1 || distance[dest] == INT_MAX)  return -1;
    return distance[dest];
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("File name is missing");
        return 0;
    }
    FILE *fin = fopen(argv[1], "r");
    if(fin == NULL){
        printf("File is not opened.\n");
        return 0;
    }
    char **s_maze = read_file(fin);
    #ifdef DEBUG
        printf("File: %s\n", argv[1]);
        for(int i = 0; i < chars; i++){
            printf("%s\n", s_maze[i]);
        }
    #endif
    int start = get_start_position(s_maze);
    if(start != -1){
        struct Graph *graph = construct_graph(s_maze);
        printf("%d\n", get_path_distance(graph, start));
        return 0;
    }
    return 0;
}
