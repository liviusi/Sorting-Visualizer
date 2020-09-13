#include <stdlib.h> // malloc(), free() etc
#include <stdio.h> // needed for testing
#include <time.h> // for randomness
#include <unistd.h> // sleep()
#include <GL/glew.h>
#include <GL/freeglut.h>

#define WINDOW_TITLE "SORTING VISUALIZER - 1.0" // window title
#define WINDOW_WIDTH 720 // self explanatory
#define WINDOW_HEIGHT 480 // self explanatory
#define DELAY 100 // every time something is swapped, wait 100 microseconds

int** ptrArray; // using the array as a global variable would've been bad
long* dim; // using array length as a global variable would've been bad
void (*sort)(void); // sorting algorithm used
// initializes array given its dimension 
// which is passed as a command line argument
int* instantiateArray();
// used for testing, prints the array using the global variables
// and warns the user if something went wrong with seeding
void printArray(int**);
// used for testing, checks whether the array has been
// correctly sorted
void isOrdered(int**);
// instantiate a window
void graphicsSetup();
// defining a way to handle keyboard inputs 
// using GLU predefined standard inputs
void keyboardEvent(unsigned char, int, int);
// used to render array elements while sorting
void renderFunction();
// swap two values and render again
void renderSwap(int*, int*);
// partition is a function used in quicksort
int partition(size_t, size_t, size_t,  int*);
// basic quicksort implementation
void auxquicksort(size_t, size_t, int*);
// first call to quicksort
void quicksort();
// insertion sort implementation
void insertionsort();
// selection sort implementation
void selectionsort();
// handles drawing and sorting
void DRAWANDSORT(int, char**, void*);



int main(int argc, char* argv[])
{
    if ( argc <= 1 )
    {
        printf("Not enough arguments were provided. Define array size.\n");
        exit(EXIT_FAILURE);
    }

    long n = strtol(argv[1], NULL, 10); // array dimension needs 
                                        // to be passed as a parameter in run.sh
    if ( n <= 0 ) // it cannot be less than 0
    {
        printf("Argument needs to be a positive integer.\n");
        exit(EXIT_FAILURE);
    }
    dim = &n;
    srand(time(NULL)); // seeding
    unsigned int inval; // input from user
    while (1)
    {
        printf("What algorithm do you want to visualize? Choose.\n");
        printf("Write 1 for insertion sort, 2 for selection sort, 3 for quicksort");
        printf(", 0 to quit.\n");
        printf("After array is displayed press s to sort, q to quit when sorted.\n");
        scanf("%d", &inval);
        switch (inval)
        {
            case 0:
                exit(EXIT_SUCCESS);
                break;
            case 1:
                DRAWANDSORT(argc, argv, insertionsort);
                break;
            case 2:
                DRAWANDSORT(argc, argv, selectionsort);
                break;
            case 3:
                DRAWANDSORT(argc, argv, quicksort);
                break;
            default:
                printf("Input is not valid.\n");
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

int* instantiateArray()
{
    int *arr = (int*) malloc(sizeof(int) * (*dim));
    for ( size_t i = 0; i < *dim; i++ )
        arr[i] = rand() % (*dim); // want integers in range [0;dim[
    return arr;
}

void printArray(int** pointerToArray)
{
    size_t i;
    int tmp;
    printf("\n\n");
    for ( i = 0; i + 1 < *dim; i++ )
    {
        tmp = (*pointerToArray)[i];
        if ( tmp < *dim )
            printf("%d,", tmp);
        else
            printf("\n\n SOMETHING WENT WRONG!! \n\n");
    }
    printf("%d.\n", (*pointerToArray)[i]);
    return;
}

void isOrdered(int **pointerToArray)
{
    for ( size_t i = 0; i + 1 < (*dim); i++ )
        if ( (*pointerToArray)[i] > (*pointerToArray)[i+1] )
        {
            printf("\n\nSOMETHING WENT WRONG WITH SORTING!\n\n");
            return;
        }
    printf("\n\nSORTED SUCCESSFULLY!\n\n");
}

void graphicsSetup()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
}

void keyboardEvent(unsigned char input, int x, int y)
{
    switch (input)
    {
        case 'q':
        case 'Q':
        case 27: // esc key
            isOrdered(ptrArray);
            exit(EXIT_SUCCESS);
            break;
        
        case 's':
        case 'S':
            sort();
            break;

        default:
            break;
    }
}

void renderFunction()
{
    graphicsSetup();
	
	float len = (float) (*dim); // need it as a float
	float auxWidth = 1 / len; // how wide a rectangle will be

	for( size_t i = 0; i < (*dim); i++ )
	{
		glBegin(GL_POLYGON);
		
		float height = 2 * ((*ptrArray)[i] + 1) / len;
		float width =  2 * i / len;

		float left   = -1 + width;
		float right  = left + auxWidth;
		float low = -1;
		float up    = low + height;

		// bottom left
		glColor4f(1,0,0,0);
		glVertex2f(left, low);

		// bottom right
		glColor4f(0,1,0,0);
		glVertex2f(right, low);

		// top right
		glColor4f(0,0,1,0);
		glVertex2f(right, up);

		// top left
		glColor4f(0,0,0,1);
		glVertex2f(left, up);

		glEnd();
	}

	glFlush();
}

void renderSwap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
    renderFunction();
    usleep(DELAY);
}

int partition(size_t left, size_t right, size_t posPivot,  int* arr)
{
    int i, j;

    int pivot = arr[posPivot];
    
    // moving pivot to last position in array
    arr[posPivot] = arr[right];
    arr[right] = pivot;

    i = left - 1;
    for( j = left ; j < right; j++ ) 
    {   // pivot lies in right partition
        if( arr[j] <= pivot ) 
        {
            i++;
            renderSwap(&(arr[i]), (&arr[j]));
        }
    }

    // swapping i+1 with right makes it go in the right position
    renderSwap(&(arr[i+1]), &(arr[right]));

    return i+1;
}

void auxquicksort(size_t left, size_t right, int* arr)
{
    if ( left < right )
    {
        int rnd = rand() % (right - left + 1) + left;

        int pivot = partition(left, right, rnd, arr);
        auxquicksort(left, pivot - 1, arr);
        auxquicksort(pivot + 1, right, arr);
    }
}

void quicksort()
{
    auxquicksort(0, *dim, *ptrArray);
}

void insertionsort()
{
    size_t j;
    size_t i = 1;
    while ( i < (*dim) )
    {
        j = i;
        while ( j > 0 && (*ptrArray)[j-1] > (*ptrArray)[j] )
        {
            renderSwap(&((*ptrArray)[j]), &((*ptrArray)[j-1]));
            j--;
        }
        i++;
    }
}

void selectionsort()
{
    size_t i, j;
    for ( i = 0; i + 1 < (*dim); i++ )
    {
        int jmin = i;
        for ( j = i + 1; j < (*dim); j++ )
            if ( (*ptrArray)[j] < (*ptrArray)[jmin] )
                jmin = j;
        if ( jmin != i )
            renderSwap(&((*ptrArray)[jmin]), &((*ptrArray)[i]));
    }
}

void DRAWANDSORT(int argc, char* argv[], void (*sortingAlgorithm))
{
    sort = sortingAlgorithm;
    int *arr = instantiateArray(); // creating array
    ptrArray = &arr; // good way to pass around global variables and 
                     // makes array accessible by renderFunction
    printArray(ptrArray); // testing
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(100,100);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(renderFunction);
	glutKeyboardFunc(keyboardEvent);

	glutMainLoop();
    free(arr);
}