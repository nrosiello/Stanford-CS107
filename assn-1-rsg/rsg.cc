/**
 * File: rsg.cc
 * ------------
 * Provides the implementation of the full RSG application, which
 * relies on the services of the built-in string, ifstream, vector,
 * and map classes as well as the custom Production and Definition
 * classes provided with the assignment.
 */
 
#include <map>
#include <fstream>
#include "definition.h"
#include "production.h"
using namespace std;

/**
 * Takes a reference to a legitimate infile (one that's been set up
 * to layer over a file) and populates the grammar map with the
 * collection of definitions that are spelled out in the referenced
 * file.  The function is written under the assumption that the
 * referenced data file is really a grammar file that's properly
 * formatted.  You may assume that all grammars are in fact properly
 * formatted.
 *
 * @param infile a valid reference to a flat text file storing the grammar.
 * @param grammar a reference to the STL map, which maps nonterminal strings
 *                to their definitions.
 */

static void readGrammar(ifstream& infile, map<string, Definition>& grammar)
{
  while (true) {
    string uselessText;
    getline(infile, uselessText, '{');
    if (infile.eof()) return;  // true? we encountered EOF before we saw a '{': no more productions!
    infile.putback('{');
    Definition def(infile);
    grammar[def.getNonterminal()] = def;
  }
}

/**
 * Determines if a given string is a nonterminal.
 */
static bool isNonTerm(string word)
{
  return word[0] == '<';
}

/**
 * Expands a given non-terminal recursively.
 */
static void expandNonTerm(vector<string>& iter, 
    map<string, Definition> grammar, 
    string nonTerm)
{
  Definition def = grammar[nonTerm];
  Production randomProd = def.getRandomProduction();

  vector<string>::iterator curr;
  for (curr = randomProd.begin(); curr < randomProd.end(); curr++) {
    string word = *curr;
    if (isNonTerm(word)) {
      expandNonTerm(iter, grammar, word);
    } else {
      iter.push_back(word);
    }
  }
}

/**
 * Populates a vector of strings with the result of a single iteration
 * of the random sentence generator for the given grammar file.
 * The vector is cleared in case it is non-empty.
 */
static void populateIter(vector<string>& iter, map<string, Definition>& grammar)
{
  iter.clear();
  expandNonTerm(iter, grammar, "<start>");
}

/**
 * Displays the result of an iteration.
 */
static void displayIter(vector<string>& iter)
{
  int lineLimit = 60;
  int currLineSize = 0;

  for (int i = 0; i < iter.size(); i++) {
    if ((currLineSize + iter[i].size()) > lineLimit) {
      cout << endl;
      currLineSize = 0;
    } else {
      currLineSize += iter[i].size();
    }

    cout << iter[i] << " ";
  }
  cout << endl;
}

/**
 * Performs the rudimentary error checking needed to confirm that
 * the client provided a grammar file.  It then continues to
 * open the file, read the grammar into a map<string, Definition>,
 * and then print out the total number of Definitions that were read
 * in.  You're to update and decompose the main function to print
 * three randomly generated sentences, as illustrated by the sample
 * application.
 *
 * @param argc the number of tokens making up the command that invoked
 *             the RSG executable.  There must be at least two arguments,
 *             and only the first two are used.
 * @param argv the sequence of tokens making up the command, where each
 *             token is represented as a '\0'-terminated C string.
 */

int main(int argc, char *argv[])
{
  if (argc == 1) {
    cerr << "You need to specify the name of a grammar file." << endl;
    cerr << "Usage: rsg <path to grammar text file>" << endl;
    return 1; // non-zero return value means something bad happened 
  }
  
  ifstream grammarFile(argv[1]);
  if (grammarFile.fail()) {
    cerr << "Failed to open the file named \"" << argv[1] << "\".  Check to ensure the file exists. " << endl;
    return 2; // each bad thing has its own bad return value
  }
  
  // things are looking good...
  map<string, Definition> grammar;
  readGrammar(grammarFile, grammar);
  cout << "The grammar file called \"" << argv[1] << "\" contains "
       << grammar.size() << " definitions." << endl;

  // generate and display the three grammar iterations
  vector<string> iter;
  for (int i = 1; i <= 3; i++) {
    cout << "Version #" << i << ": -------------------" << endl;
    populateIter(iter, grammar);
    displayIter(iter);
  }
 
  return 0;
}
