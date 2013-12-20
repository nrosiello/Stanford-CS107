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

///------------------------------ 
struct cmpPair {
  char *key;
  const void *recBlob;
};

static char *getRecord(const void *recBlob, const void *elem)
{
  return (char *) (recBlob) + * (int *) elem;
}

static void *recNameEnd(const void *rec) {
  char *iter = (char *) rec;
  
  while (*iter) 
    iter++;
  iter++;

  if ((unsigned long) iter % 2 == 1)
    iter++;
  return iter;
}

film imdb::movieRecToFilm(const void *movieRec) const
{
  string title = (char *) movieRec;
  unsigned char yearDelta = *(unsigned char *) recNameEnd(movieRec);
  film filmStr = { title, 1900 + yearDelta };
  return filmStr;
}

int actorCmpFn(const void *keyPtr, const void *elem)
{
  cmpPair keyPair = * (cmpPair *) keyPtr;
  char *elemName = getRecord(keyPair.recBlob, elem);
  return strcmp(keyPair.key, elemName);
}

void imdb::extractFilms(const void *elem, vector<film>& films) const
{
  void *actorRec = getRecord(actorFile, elem);
  short *numCredits = (short *) recNameEnd(actorRec);
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

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const
{ 
  int numActors = * (int *) actorFile;
  int *base = (int *) actorFile + 1;
 
  // create a pair that needs to be passed into bsearch containing the
  // actor name and a pointer to the record blob 
  char key[256];
  strcpy(key, player.c_str());
  cmpPair keyPair = { key, actorFile };
  
  void *found = bsearch(&keyPair, base, numActors, sizeof(int), actorCmpFn);

  if (found) {
    extractFilms(found, films);
    return true;
  } else { 
    return false;
  }
}

bool imdb::getCast(const film& movie, vector<string>& players) const { 
 return false;
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
