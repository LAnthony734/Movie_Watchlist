/*
// Main.c - this file contains all relevent code for the Movie Watchlist project (see documentation)
//
// The MIT License (MIT)
// 
// Copyright (c) 2021 Luke Andrews.  All Rights Reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify, merge,
// publish, distribute, sub-license, and/or sell copies of the Software, and to permit persons
// to whom the Software is furnished to do so, subject to the following conditions:
// 
// * The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
*/
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

/*
// Encapsulates a Movie.
*/
typedef struct Movie
{
	char          title[35];
	char          genre[35];
	double        duration;  /* hours */
	struct Movie* next;
} Movie;

/*
// Page break.
*/
void pageBreak()
{
	printf("********************************************************************************************\n");
	printf("\n");
}

/*
// Dynamically allocates and creates a Movie.
//
// [in] title	 - The title of the Movie
// [in] genre    - The genre of the Movie
// [in] duration - The duration of the Movie in hours
//
// Returns the created movie, or NULL on error.
*/
Movie* createMovieNode(char* title, char* genre, double duration)
{
	int    status = 0;
	Movie* movie  = NULL;
	int    titleLength = 0;
	int    genreLength = 0;

	if (status == 0)
	{
		if (title == NULL || genre == NULL)
		{
			status = EINVAL;
		}
		else if ((titleLength = strlen(title)) > _countof(movie->title) - 1)
		{
			status = ERANGE;
		}
		else if ((genreLength = strlen(genre)) > _countof(movie->genre) - 1)
		{
			status = ERANGE;
		}
	}

	if (status == 0)
	{
		movie = calloc(1, sizeof(Movie));

		if (movie == NULL)
		{
			status = ENOMEM;
		}
	}

	if (status == 0)
	{
		if (title != NULL || strlen(title) > 35)
		{
			strcpy(movie->title, title);
		}

		if (genre != NULL || strlen(title) > 35)
		{
			strcpy(movie->genre, genre);
		}

		movie->duration = duration;
	}

	if (status != 0)
	{
		free(movie);
		movie = NULL;
		errno = status;
	}

	return movie;
}

/*
// Prompts for a string value.
//
// [in] buffer     - The input buffer
// [in] bufferSize - The byte size of the input buffer
// [in] prompt     - The prompt printed to the user
// [in] ...        - Additional arguments for a formatted prompt
//
// Returns the length of the input string, or EOF on error.
*/
int promptFor(char* buffer, int bufferSize, const char* prompt, ...)
{
	int newLength      = EOF;
	int originalLength = 0;
	int character      = 0;

	if (buffer != NULL && bufferSize > 0)
	{
		*buffer = '\0';

		if (prompt != NULL)
		{
			va_list alist;
			va_start(alist, prompt);

			vprintf(prompt, alist);

			va_end(alist);
		}

		if (fgets(buffer, bufferSize, stdin))
		{
			/*
			// Trim the trailing newlines from the input string:
			*/
			originalLength = strlen(buffer);

			newLength = originalLength;

			while (newLength > 0 && buffer[newLength - 1] == '\n')
			{
				buffer[--newLength] = '\0';
			}

			/*
			// Clear the input buffer if we didn't see a newline:
			*/
			if (originalLength == newLength)
			{
				while (!feof(stdin))
				{
					character = fgetc(stdin);

					if (character <= EOF || character == '\n')
					{
						break;
					}
				}
			}
		}
	}

	return newLength;
}

/*
// Prompts for an integer between a given range (inclusively).
//
// [in] minValue - The minimum value allowed
// [in] maxValue - The maximum value allowed
// [in] prompt   - The prompt printed to the user
//
// Returns the input integer.
*/
int promptForInt(int minValue, int maxValue, const char* prompt)
{
	char  buffer[100] = {0};
	char* remainder   = buffer;
	int   integer     = 0;

	while (true)
	{
		promptFor(buffer, _countof(buffer), prompt);

		integer = strtol(buffer, &remainder, 10);

		if (remainder == buffer || *remainder != '\0' || integer < minValue || integer > maxValue)
		{
			printf("\n");
			printf("An integer between %d and %d was expected.\n", minValue, maxValue);
			printf("\n");
		}
		else
		{
			break;
		}
	}

	return integer;
}

/*
// Determines the count of Movies in a given linked list.
//
// [in] list - The linked list of Movies
//
// Returns the count of Movies.
*/
int getCount(Movie* list)
{
	int count = 0;

	if (list != NULL)
	{
		++count;

		while (list->next != NULL)
		{
			++count;
			list = list->next;
		}
	}

	return count;
}

/*
// Inserts a given Movie into a given linked list at a given position (zero-based).
//
// [in] list     	- The linked list of Movies
// [in] insertMovie - The Movie to insert
// [in] position    - The position in the linked list at which to insert
//
// Returns error status code.
*/
int insertMovie(Movie** list, Movie* insertMovie, int position)
{
	int    status = 0;
	Movie* prev   = NULL;
	Movie* itr    = NULL;
	int    count  = 0;

	if (status == 0)
	{
		if (list == NULL || insertMovie == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		itr = *list;

		while (true)
		{
			if (count == position)
			{
				if (prev != NULL)
				{
					prev->next = insertMovie;
				}

				insertMovie->next = itr;

				if (position == 0)
				{
					*list = insertMovie;
				}

				break;
			}
			else if (itr->next == NULL)
			{
				status = ERANGE;
				break;
			}
			else
			{
				prev = itr;
				itr  = itr->next;
				++count;
			}
		}
	}

	return status;
}

/*
// Appends a given Movie to the end of a given linked.
//
// [in] list     	- The linked list of Movies
// [in] appendMovie - The Movie to append
//
// Returns error status code.
*/
int appendMovie(Movie** list, Movie* appendMovie)
{
	int    status = 0;
	Movie* head   = NULL;

	if (status == 0)
	{
		if (list == NULL || appendMovie == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		head = *list;

		if (head == NULL)
		{
			head  = appendMovie;
			*list = appendMovie;
		}
		else
		{
			while (head->next != NULL)
			{
				head = head->next;
			}

			head->next = appendMovie;
		}

		appendMovie->next = NULL;
	}

	return status;
}

/*
// Removes a given Movie from a given linked list of Movies.
//
// [in] list	    - The linked list of Movies
// [in] removeMovie - The Movie to remove
//
// Returns the removed Movie, or NULL on error.
*/
Movie* removeMovie(Movie** list, Movie* removeMovie)
{
	int    status       = 0;
	Movie* removedMovie = NULL;
	Movie* prev         = NULL;
	Movie* itr          = NULL;

	if (status == 0)
	{
		if (list == NULL || *list == NULL || removeMovie == NULL)
		{
			status       = EINVAL;
			removedMovie = NULL;
		}
	}

	if (status == 0)
	{
		if (removeMovie == *list)
		{
			*list        = removeMovie->next;
			removedMovie = removeMovie;
		}
		else
		{
			prev = *list;
			itr = prev->next;

			while (true)
			{
				if (itr == removeMovie)
				{
					prev->next   = itr->next;
					removedMovie = itr;
					itr->next    = NULL;
					break;
				}

				if (itr->next == NULL)
				{
					status = EINVAL;
					break;
				}

				prev = itr;
				itr  = itr->next;
			}
		}
	}

	return removedMovie;
}

/*
// Removes a given Movie from a given linked list of Movies, and deletes it from memory.
//
// [in] list		- The linked list of Movies
// [in] deleteMovie - The Movie to delete
//
// Returns 0 on success, -1 on failure.
*/
int deleteMovie(Movie** list, Movie* deleteMovie)
{
	int    status = 0;
	Movie* prev   = NULL;
	Movie* itr    = NULL;

	if (status == 0)
	{
		if (list == NULL || *list == NULL || deleteMovie == NULL)
		{
			status = -1;
		}
	}

	if (status == 0)
	{
		if (deleteMovie == *list)
		{
			*list = deleteMovie->next;
			free(deleteMovie);
		}
		else
		{
			prev = *list;
			itr = prev->next;

			while (true)
			{
				if (itr == deleteMovie)
				{
					prev->next  = itr->next;
					itr->next   = NULL;
					free(deleteMovie);
					break;
				}

				if (itr->next == NULL)
				{
					status = -1;
					break;
				}

				prev = itr;
				itr  = itr->next;
			}
		}
	}

	return status;
}

/*
// Deletes a given linked list of Movies from memory.
//
// [in] list - The linked list of Movies
//
// Returns 0 on success, -1 on failure.
*/
int deleteList(Movie** list)
{
	int    status = 0;
	Movie* next   = NULL;
	Movie* itr    = NULL;

	if (status == 0)
	{
		if (list == NULL || *list == NULL)
		{
			status = -1;
		}
	}

	if (status == 0)
	{
		itr  = *list;
		
		while (true)
		{
			next = itr->next;

			free(itr);

			if (next == NULL)
			{
				break;
			}

			itr = next;
		}
	}

	return status;
}

/*
// Prints a given Movie.
//
// [in] movie - The Movie to print
*/
void printMovie(Movie* movie)
{
	int status = 0;

	if (status == 0)
	{
		if (movie == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		printf("%s (%s, %.2f hours)\n", movie->title, movie->genre, movie->duration);
	}

	if (status != 0)
	{
		errno = status;
	}
}

/*
// Prints the Movies in a given linked list of Movies.
//
// [in] list - The linked list of Movies to print
*/
void printMovieList(Movie* list)
{
	int    status = 0;
	Movie* itr    = NULL;

	if (status == 0)
	{
		if (list == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		itr = list;

		while (true)
		{
			printMovie(itr);

			if (itr->next == NULL)
			{
				break;
			}

			itr = itr->next;
		}
	}
	printf("\n");
}

/*
// Computes the total duration in hours of a given linked list of movies.
//
// [in] list - The linked list of Movies
//
// Returns the computed duration.
*/
double computeDuration(Movie* list)
{
	int    status   = 0;
	double duration = 0.0;
	Movie* itr      = NULL;

	if (status == 0)
	{
		if (list == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		itr = list;

		while (true)
		{
			duration += itr->duration;

			if (itr->next == NULL)
			{
				break;
			}

			itr = itr->next;
		}
	}

	return duration;
}

/*
// Searches for a Movie by its given title in a given linked list of Movies.
//
// [in] list  - The linked list of Movies
// [in] title - The title of the Movie to search for
//
// Returns the Movie with the given title, or NULL if not found.
*/
Movie* searchByTitle(Movie* list, char* title)
{
	int    status = 0;
	Movie* movie  = NULL;
	Movie* itr    = NULL;

	if (status == 0)
	{
		if (list == NULL || title == NULL)
		{
			status = EINVAL;
			movie  = NULL;
		}
	}

	if (status == 0)
	{
		itr = list;

		while (true)
		{
			if (strcmp(itr->title, title) == 0)
			{
				movie = itr;
				break;
			}

			if (itr->next == NULL)
			{
				break;
			}

			itr = itr->next;
		}
	}

	return movie;
}

/*
// Determines the position (zero-based) of a Movie by its given title in a given linked list of Movies.
//
// [in] list  - The linked list of Movies
// [in] title - The title of the Movie to find position
//
// Returns the position of the Movie, or -1 if not found.
*/
int getNodePosition(Movie* list, char* title)
{
	int    position = -1;
	int    index    = 0;
	int    status   = 0;
	Movie* itr      = NULL;

	if (status == 0)
	{
		if (list == NULL || title == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		index = 0;
		itr   = list;

		while (true)
		{
			if (strcmp(itr->title, title) == 0)
			{
				position = index;
				break;
			}

			if (itr->next == NULL)
			{
				break;
			}

			itr = itr->next;
		}
	}
	
	return position;
}

/*
// Enum of all options for adding a movie.
*/
typedef enum AddMovieMenuOption
{
	AddToBeginning = 1,
	AddToEnd       = 2,
	InsertWithin   = 3
} AddMovieMenuOption;

/*
// Prints the menu for adding a movie.
*/
void printAddMovieMenu()
{
	printf("*** Add Movie Menu ***\n");
	printf(" 1) Add to beginning\n");
	printf(" 2) Add to end\n");
	printf(" 3) Insert at a position\n");
	printf("\n");
}

/*
// Prompts for and returns a menu option for adding a movie.
*/
AddMovieMenuOption getAddMovieMenuOption()
{
	AddMovieMenuOption option = 0;

	printAddMovieMenu();

	option = promptForInt(1, 3, "Enter how you'd like to add: ");
	printf("\n");

	return option;
}

/*
// Handles a single menu option for adding a movie.
//
// [in] option    - The menu option
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
*/
void handleAddMovieMenuOption(AddMovieMenuOption option, Movie* movie, Movie** library, Movie** watchlist)
{
	int    position = 0;
	int    count    = 0;
	Movie* newMovie = NULL;

	newMovie = createMovieNode(movie->title, movie->genre, movie->duration);

	switch (option)
	{
		case AddToBeginning:
		{
			insertMovie(watchlist, newMovie, 0);
			removeMovie(library, movie);
			break;
		}

		case AddToEnd:
		{
			appendMovie(watchlist, newMovie);
			removeMovie(library, movie);
			break;
		}

		case InsertWithin:
		{
			count = getCount(*watchlist);

			printf("Enter a position from 1 to %d to add the movie: ", count);
			printf("\n");
			position = promptForInt(1, count, "");

			insertMovie(watchlist, newMovie, position - 1);
			removeMovie(library, movie);
			break;
		}

		default:
		{
			fprintf(stderr, "Unhandled menu option for adding a movie.\n");
			free(newMovie);
		}
	}
}

/*
// Handles menu selection and operations for adding a movie.
//
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
*/
void handleAddMovie(Movie** library, Movie** watchlist)
{
	AddMovieMenuOption option    = 0;
	char               title[35] = {0};
	Movie*             movie     = NULL;

	promptFor(title, sizeof(title), "Enter the title of the movie to add: ");
	printf("\n");
	movie = searchByTitle(*library, title);

	if (movie != NULL)
	{
		option = getAddMovieMenuOption();

		handleAddMovieMenuOption(option, movie, library, watchlist);

		printf("%s added to the watchlist.\n", title);
		printf("\n");
	}
	else
	{
		printf("%s not found in the library. Please search for movies before attempting to add.\n", title);
		printf("\n");
	}
}

/*=========================================================================================================
// Movie Library
//=======================================================================================================*/

/*
// Enum of all library menu options.
*/
typedef enum LibraryMenuOption
{
	ViewAllMovies       = 1,
	SearchLibrary       = 2,
	AddMovieToWatchlist = 3,
	BackToWatchlist     = 4
} LibraryMenuOption;

/*
// Prompts for the name of a text file and stores the contents as a linked list of Movies.
//
// Returns the linked list of Movies, or NULL on error.
*/
Movie* loadMovieLibrary(char* fileName)
{
	int    status    = 0;
	FILE*  input     = NULL;
	Movie* library   = NULL;
	Movie* movie     = NULL;
	char   title[35] = {0};
	char   genre[35] = {0};
	double duration  = 0.0;
	char   line[100] = {0};

	if (fileName == NULL)
	{
		status = EINVAL;
	}

	if (status == 0)
	{
		input = fopen(fileName, "r");
		
		if (input == NULL)
		{
			status = errno;
		}
	}

	if (status == 0)
	{
		while (!feof(input))
		{
			fgets(title, _countof(title), input);
			title[strlen(title) - 1] = '\0';
			
			fgets(genre, _countof(genre), input);
			genre[strlen(genre) - 1] = '\0';

			fgets(line, _countof(line), input);
			duration = strtod(line, NULL);

			movie = createMovieNode(title, genre, duration);

			if (movie == NULL)
			{
				status = errno;	
				break;
			}
			else
			{
				status = appendMovie(&library, movie);
			}
		}
	}

	if (status != 0)
	{
		deleteList(&library);
		errno = status;
	}

	fclose(input);

	return library;
}

/*
// Prints the library menu.
*/
void printLibraryMenu()
{
	printf("*** Library Menu ***\n");
	printf("1) View all movies\n");
	printf("2) Search by title\n");
	printf("3) Add a movie to watchlist\n");
	printf("4) Back to watchlist\n");
	printf("\n");
}

/*
// Prompts for and returns a library menu option choice.
*/
LibraryMenuOption getLibraryMenuOption()
{
	LibraryMenuOption option = 0;

	printLibraryMenu();

	option = promptForInt(1, 4, "Enter a menu choice: ");
	printf("\n");

	return option;
}

//
// Handles a single library menu option.
//
// [in] option    - The library menu option
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
//
void handleLibraryMenuOption(LibraryMenuOption option, Movie** library, Movie** watchlist)
{
	char title[35] = {0};

	switch (option)
	{
		case ViewAllMovies:
		{
			printMovieList(*library);
			break;
		}

		case SearchLibrary:
		{
			promptFor(title, sizeof(title), "Enter a title to search: ");
			printf("\n");

			if (searchByTitle(*library, title) != NULL)
			{
				printf("%s found in the library.\n", title);
				printf("\n");
			}
			else
			{
				printf("%s not found in the library.\n", title);
				printf("\n");
			}
			break;
		}

		case AddMovieToWatchlist:
		{
			pageBreak();

			handleAddMovie(library, watchlist);
			break;
		}

		default:
		{
			fprintf(stderr, "Unhandled Library option.\n");
		}
	}
}

/*
// Handles library menu selection and operations.
//
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
*/
void handleLibrary(Movie** library, Movie** watchlist)
{
	LibraryMenuOption option = 0;

	while (true)
	{
		option = getLibraryMenuOption();

		if (option == BackToWatchlist)
		{
			break;
		}

		handleLibraryMenuOption(option, library, watchlist);

		pageBreak();
	}
}

/*=========================================================================================================
// Movie Watchlist
//=======================================================================================================*/

/*
// Enum of all Watchlist menu options.
*/
typedef enum WatchlistMenuOption
{
	PrintWatchlist  = 1,
	ShowDuration    = 2,
	SearchWatchlist = 3,
	MoveMovieUp     = 4,
	MoveMovieDown   = 5,
	RemoveMovie     = 6,
	SaveWatchlist   = 7,
	LoadWatchlist   = 8,
	GoToLibrary     = 9,
	Quit            = 10
} WatchlistMenuOption;

/*
// Prompts for the name of a text file and stores the given linked list of Movies into that text file.
//
// [in] list - The linked list of Movies
//
// Returns error status code.
*/
int saveMovieWatchlist(Movie* list)
{
	int   status        = 0;
	char  fileName[200] = {0};
	FILE* output        = NULL;

	if (status == 0)
	{
		if (list == NULL)
		{
			status = EINVAL;
		}
	}

	if (status == 0)
	{
		promptFor(fileName, sizeof(fileName), "Enter the name of the file to save watchlist to: ");
		printf("\n");

		output = fopen(fileName, "w");

		if (output == NULL)
		{
			status = errno;
		}
	}

	if (status == 0)
	{
		while (true)
		{
			fprintf(output, "%s\n%s\n%.2f", list->title, list->genre, list->duration);

			if (list->next == NULL)
			{
				break;
			}

			fprintf(output, "\n");
			list = list->next;
		}


	}

	fclose(output);

	return status;
}

//
// Prompts for the name of a text file and stores the contents as a linked list of Movies.
//
// Returns the linked list of Movies, or NULL on error.
//
Movie* loadMovieWatchlist(Movie** library)
{
	int    status        = 0;
	char   fileName[200] = {0};
	FILE*  input         = NULL;
	char   line[100]     = {0};
	Movie* watchlist     = NULL;
	Movie* temp          = NULL;
	Movie* movie         = NULL;
	char   title[35]     = {0};
	char   genre[35]     = {0};
	double duration      = 0.0;

	if (status == 0)
	{
		promptFor(fileName, sizeof(fileName), "Enter the name of the file to read the watchlist from: ");
		printf("\n");

		input = fopen(fileName, "r");

		if (input == NULL)
		{
			status = errno;
		}
	}

	if (status == 0)
	{
		while (!feof(input))
		{
			fgets(title, _countof(title), input);
			title[strlen(title) - 1] = '\0';

			fgets(genre, _countof(genre), input);
			genre[strlen(genre) - 1] = '\0';

			fgets(line, _countof(line), input);
			duration = strtod(line, NULL);

			movie = createMovieNode(title, genre, duration);

			if (movie == NULL)
			{
				status = errno;
				break;
			}
			else
			{
				appendMovie(&watchlist, movie);

				temp = searchByTitle(*library, movie->title);

				if (temp != NULL)
				{
					removeMovie(library, temp);
				}
			}
		}
	}

	fclose(input);

	return watchlist;
}

/*
// Prints the Watchlist menu.
*/
void printWatchlistMenu()
{
	printf("*** Watchlist Menu ***\n");
	printf(" 1) Print watchlist\n");
	printf(" 2) Show duration\n");
	printf(" 3) Search by title\n");
	printf(" 4) Move a movie up\n");
	printf(" 5) Move a movie down\n");
	printf(" 6) Remove a movie\n");
	printf(" 7) Save watchlist\n");
	printf(" 8) Load watchlist\n");
	printf(" 9) Go to movie library\n");
	printf("10) Quit\n");
	printf("\n");
}

/*
// Prompts for and returns a Watchlist menu option choice.
*/
WatchlistMenuOption getWatchlistMenuOption()
{
	WatchlistMenuOption option = 0;

	printWatchlistMenu();

	option = promptForInt(1, 10, "Enter a menu choice: ");
	printf("\n");

	return option;
}

//
// Handles a single Watchlist menu option.
//
// [in] option    - The library menu option
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
//
void handleWatchlistMenuOption(WatchlistMenuOption option, Movie** library, Movie** watchlist)
{
	char   title[35] = {0};
	Movie* temp      = NULL;
	Movie* newMovie  = NULL;
	Movie* itr       = NULL;

	switch (option)
	{
		case PrintWatchlist:
		{
			printMovieList(*watchlist);
			break;
		}

		case ShowDuration:
		{
			printf("Duration is %.2f hours.\n", computeDuration(*watchlist));
			printf("\n");
			break;
		}

		case SearchWatchlist:
		{
			promptFor(title, sizeof(title), "Enter a title to search: ");
			printf("\n");

			if (searchByTitle(*watchlist, title) != NULL)
			{
				printf("%s found in the watchlist.\n", title);
				printf("\n");
			}
			else
			{
				printf("%s not found in the watchlist.\n", title);
				printf("\n");
			}
			break;
		}

		case MoveMovieUp:
		{
			promptFor(title, sizeof(title), "Enter the title of the movie to move up: ");
			printf("\n");

			if (searchByTitle(*watchlist, title) != NULL)
			{
				itr = *watchlist;

				if (strcmp(itr->title, title) != 0)
				{
					while (true)
					{
						if (strcmp(itr->next->title, title) == 0)
						{
							if (itr == *watchlist)
							{
								*watchlist = itr->next;
							}

							temp            = itr->next->next;
							itr->next->next = itr;
							itr->next       = temp;

							break;
						}

						itr = itr->next;
					}
				}
			}
			else
			{
				printf("%s not found in the watchlist. Please search for movies before attempting to move.\n", title);
				printf("\n");
			}
			break;
		}

		case MoveMovieDown:
		{
			promptFor(title, sizeof(title), "Enter the title of the movie to move down: ");
			printf("\n");

			if (searchByTitle(*watchlist, title) != NULL)
			{
				itr = *watchlist;

				while (itr->next != NULL)
				{
					if (strcmp(itr->title, title) == 0)
					{
						if (itr == *watchlist)
						{
							*watchlist = itr->next;
						}

						temp            = itr->next->next;
						itr->next->next = itr;
						itr->next       = temp;

						break;
					}

					itr = itr->next;
				}
			}
			else
			{
				printf("%s not found in the watchlist. Please search for movies before attempting to move.\n", title);
				printf("\n");
			}
			break;
		}

		case RemoveMovie:
		{
			promptFor(title, sizeof(title), "Enter the title of the movie to remove: ");
			printf("\n");
			temp = searchByTitle(*watchlist, title);

			if (temp != NULL)
			{
				newMovie = createMovieNode(temp->title, temp->genre, temp->duration);
				removeMovie(watchlist, temp);
				appendMovie(library, newMovie);
			}
			else
			{
				printf("%s not found in the watchlist. Please search for movies before attempting to remove.\n", title);
				printf("\n");
			}

			break;
		}

		case SaveWatchlist:
		{
			saveMovieWatchlist(*watchlist);
			break;
		}

		case LoadWatchlist:
		{
			*watchlist = loadMovieWatchlist(library);
			break;
		}

		case GoToLibrary:
		{
			pageBreak();

			handleLibrary(library, watchlist);
			break;
		}

		default:
		{
			fprintf(stderr, "Unhandled watchlist option.\n");
		}
	}
}

/*
// Handles Watchlist menu selection and operations.
//
// [in] library   - The library of Movies
// [in] watchlist - The watchlist of Movies
*/
void handleWatchlist(Movie** library, Movie** watchlist)
{
	WatchlistMenuOption option = 0;

	while (true)
	{
		option = getWatchlistMenuOption();

		if (option == Quit)
		{
			break;
		}

		handleWatchlistMenuOption(option, library, watchlist);

		pageBreak();
	}
}

/*=========================================================================================================
//
// Program entry point. argv[1] reserved for Movie text file.
*/
int main(int argc, char** argv)
{
	int    status    = 0;
	Movie* watchlist = NULL;
	Movie* library   = NULL;

	if (status == 0)
	{
		if (argc != 2)
		{
			status = E2BIG;
		}
	}

	if (status == 0)
	{
		library = loadMovieLibrary(argv[1]);

		if (library == NULL)
		{
			status = errno;
		}
	}

	if (status == 0)
	{
		handleWatchlist(&library, &watchlist);
	}

	deleteList(&watchlist);
	deleteList(&library);

	return status;
}