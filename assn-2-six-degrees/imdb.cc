using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

/*
 * A pair is needed to be passed to both the actor and movie comparison
 * functions because both of the functions need both a pointer to the
 * memory file and a pointer to the key.
 */
struct cmpPair {
  const void *key;
  const void *recBlob;
};

/*
 * Returns a pointer to the specific actor or movie record if given a
 * memory file and integer offset in bytes.
 */
static char *getRecord(const void *recBlob, const void *offset)
{
  return (char *) (recBlob) + * (int *) offset;
}

/* 
 * Returns a pointer to the end of the name portion of an actor record 
 * (i.e. the short storing the number of movies), if given a pointer to the
 * beginning of the record.
 */
static void *actorNameEnd(const void *rec) {
  char *iter = (char *) rec;
  
  while (*iter) 
    iter++;
  iter++;

  if ((unsigned long) iter % 2 == 1)
    iter++;
  return iter;
}

/*
 * Given a pointer to a movie record, returns a film containing the title
 * and year of the movie.
 */
film movieRecToFilm(const void *movieRec) 
{
  string title = (char *) movieRec;
  unsigned char yearDelta = *(unsigned char *) actorNameEnd(movieRec);
  film filmStr = { title, 1900 + yearDelta };
  return filmStr;
}

/*
 * Comparison function to be used when searching the actor array.
 * The key is an actor name.
 */
int actorCmpFn(const void *keyPtr, const void *elem)
{
  cmpPair keyPair = * (cmpPair *) keyPtr;
  char *elemName = getRecord(keyPair.recBlob, elem);
  return strcmp((*(string *) keyPair.key).c_str(), elemName);
}

/*
 * If given a pointer to an offset of an actor record, will populate the
 * given films vector with all of the films for that actor.  Any existing
 * entries in the films vector will be cleared.
 */
void imdb::extractFilms(const void *offset, vector<film>& films) const
{
  void *actorRec = getRecord(actorFile, offset);
  short *numCredits = (short *) actorNameEnd(actorRec);
  int *filmElemArray = (int *) (numCredits + 1);
  // need to move the pointer over two bytes if it is not a multiple of four
  int additionalPadding = ((char *) filmElemArray - (char *) actorRec) % 4;
  filmElemArray = (int *) ((char *) filmElemArray + additionalPadding);
  
  films.clear();
  for (int i = 0; i < *numCredits; i++) {
    void *movieRec = getRecord(movieFile, filmElemArray + i); 
    films.push_back(movieRecToFilm(movieRec));
  }
}

/*
 * Wrapper to bsearch to be used when searched memory files with formats
 * consistent with the actor and movie files.
 */
void *imdb::searchFile(const void *file, const void *key,
   int (*cmpFn)(const void *, const void *)) const
{
  int numRecords = * (int *) file;
  int *base = (int *) file + 1;

  // create a pair that needs to be passed into bsearch containing the
  //  name and a pointer to the record file 
  cmpPair keyPair = { key, file };
  return bsearch(&keyPair, base, numRecords, sizeof(int), cmpFn);
}

/*
 * Given an actor name, searches the actor file for that actor.  If not found,
 * returns false.  If found, populates the films vector with the film credits
 * for that actor and returns true.
 */
bool imdb::getCredits(const string& player, vector<film>& films) const
{ 
  void *found = searchFile(actorFile, &player, actorCmpFn);
  
  if (found) {
    extractFilms(found, films);
    return true;
  } else { 
    return false;
  }
}

/*
 * Comparison function to be used when searching the movie array.  Uses 
 * a film as the key type.
 */
int movieCmpFn(const void *keyPtr, const void *elem)
{
  cmpPair keyPair = * (cmpPair *) keyPtr;
  film key = * (film *) keyPair.key;
  void *testMovieRec = getRecord(keyPair.recBlob, elem);
  film test = movieRecToFilm(testMovieRec);
  
  if (key == test) {
    return 0;
  } else if (key < test) {
    return -1;
  } else {
    return 1;
  }
}

/*
 * Given a pointer to a movie record, populates the players vector with
 * all of the cast for that movie.
 */
void imdb::extractCast(const void *elem, vector<string>& players) const
{
  // first traverse the record to find the number of actors and start of the
  // actor record offset values 
  char *iter = (char *) elem;
  while (*iter)
    iter++;
  iter += 2; // the \0 and the one-byte year offset
  iter += (iter - (char *) elem) % 2; // padding in case the offset is odd

  short *numActors = (short *) iter;
  int *actorOffsets = (int *) (numActors + 1);
  // need to move the pointer over two bytes if it is not a multiple of four
  int additionalPadding = ((char *) actorOffsets - (char *) elem) % 4;
  actorOffsets = (int *) ((char *) actorOffsets + additionalPadding);

  players.clear(); 
  for (int i = 0; i < *numActors; i++) {
    char *actorName = (char *) actorFile + actorOffsets[i];
    players.push_back(actorName);
  }
}

/*
 * Searches the movie memory file for a given film. If not found, returns
 * false.  If found, populates the players vector with the cast and returns
 * true.
 */
bool imdb::getCast(const film& movie, vector<string>& players) const 
{ 
  void *found = searchFile(movieFile, &movie, movieCmpFn);

  if (found) {
    extractCast(getRecord(movieFile, found), players);
    return true;
  } else { 
    return false;
  }
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
