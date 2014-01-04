#!/usr/bin/env python

from __future__ import print_function
import random  # for seed, random
import sys    # for stdout
import unittest
import copy


def findOptimalAlignment(strand1, strand2, cache):
    def addToCache(aligned):
        newCacheValue = copy.deepcopy(aligned)
        cache[key] = newCacheValue

    key = strand1 + '-' + strand2
    if key in cache:
        return cache[key]

    # if one of the two strands is empty, then there is only
    # one possible alignment, and of course it's optimal
    if len(strand1) == 0:
        aligned = {'strand1': ' ' * len(strand2),
                   'strand2': strand2,
                   'score': len(strand2) * -2}
        addToCache(aligned)
        return aligned
    if len(strand2) == 0:
        aligned = {'strand1': strand1,
                   'strand2': ' ' * len(strand1),
                   'score': len(strand1) * -2}
        addToCache(aligned)
        return aligned

    # There's the scenario where the two leading bases of
    # each strand are forced to align, regardless of whether or not
    # they actually match.
    bestWith = findOptimalAlignment(strand1[1:],
                                    strand2[1:],
                                    cache)
    if strand1[0] == strand2[0]:
        # no benefit from making other recursive calls
        aligned = {'strand1': strand1[0] + bestWith['strand1'],
                   'strand2': strand2[0] + bestWith['strand2'],
                   'score': bestWith['score'] + 1}
        addToCache(aligned)
        return aligned

    best = {'strand1': strand1[0] + bestWith['strand1'],
            'strand2': strand2[0] + bestWith['strand2'],
            'score': bestWith['score'] - 1}

    # It's possible that the leading base of strand1 best
    # matches not the leading base of strand2, but the one after it.
    bestWithout = findOptimalAlignment(strand1, strand2[1:], cache)
    bestWithout['score'] -= 2  # penalize for insertion of space
    if bestWithout['score'] > best['score']:
        bestWithout['strand1'] = ' ' + bestWithout['strand1']
        bestWithout['strand2'] = strand2[0] + bestWithout['strand2']
        best = copy.deepcopy(bestWithout)

    # opposite scenario
    bestWithout = findOptimalAlignment(strand1[1:], strand2, cache)
    bestWithout['score'] -= 2  # penalize for insertion of space
    if bestWithout['score'] > best['score']:
        bestWithout['strand2'] = ' ' + bestWithout['strand2']
        bestWithout['strand1'] = strand1[0] + bestWithout['strand1']
        best = copy.deepcopy(bestWithout)

    addToCache(best)
    return best


# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
    assert minlength > 0, \
        "Minimum length passed to generateRandomDNAStrand" \
        "must be a positive number"  # these \'s allow mult-line statements
    assert maxlength >= minlength, \
        "Maximum length passed to generateRandomDNAStrand must be at " \
        "as large as the specified minimum length"
    length = random.choice(xrange(minlength, maxlength + 1))
    bases = ['A', 'T', 'G', 'C']
    return ''.join([random.choice(bases) for _ in xrange(length)])

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.


def plusChar(s1, s2):
    if s1 == s2:
        return '1'
    else:
        return ' '


def minusChar(s1, s2):
    if s1 == ' ' or s2 == ' ':
        return '2'
    elif s1 != s2:
        return '1'
    else:
        return ' '


def printAlignment(alignment, out=sys.stdout):
    print("\nOptimal alignment score is", alignment['score'], "\n")
    print("+\t", ''.join([plusChar(s1, s2) for s1, s2
                          in zip(alignment['strand1'], alignment['strand2'])]))
    print("\t", alignment['strand1'])
    print("\t", alignment['strand2'])
    print("-\t", ''.join([minusChar(s1, s2) for s1, s2
                          in zip(alignment['strand1'], alignment['strand2'])]))

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.


class FindOptimalAlignmentTest(unittest.TestCase):

    def testStrandOneEmpty(self):
        cache = {}
        correct = {'strand1': '  ', 'strand2': 'GC', 'score': -4}
        test = findOptimalAlignment('', 'GC', cache)
        self.assertDictEqual(correct, test)

    def testStrandTwoEmpty(self):
        cache = {}
        correct = {'strand1': 'GACG', 'strand2': '    ', 'score': -8}
        test = findOptimalAlignment('GACG', '', cache)
        self.assertDictEqual(correct, test)

    def testShiftStrandOne(self):
        cache = {}
        correct = {'strand1': 'ACAG', 'strand2': 'AC G', 'score': 1}
        test = findOptimalAlignment('ACAG', 'ACG', cache)
        self.assertDictEqual(correct, test)

    def testShiftStrandTwo(self):
        cache = {}
        correct = {'strand1': 'AC GAC G', 'strand2': 'ACAGACAG', 'score': 2}
        test = findOptimalAlignment('ACGACG', 'ACAGACAG', cache)
        self.assertDictEqual(correct, test)

    def testMediumStrandOne(self):
        cache = {}
        correct = {'strand1': "GCC CGTAGGG",
                   'strand2': "GCGGCGC GGG",
                   'score': 1}
        s1 = "GCCCGTAGGG"
        s2 = "GCGGCGCGGG"
        test = findOptimalAlignment(s1, s2, cache)
        self.assertDictEqual(correct, test)

    def testMediumStrandTwo(self):
        cache = {}
        correct = {'strand1': "G CC AA",
                   'strand2': "GTCCTAA",
                   'score': 1}
        s1 = "GCCAA"
        s2 = "GTCCTAA"
        test = findOptimalAlignment(s1, s2, cache)
        self.assertDictEqual(correct, test)

    def testBigStrandsOne(self):
        cache = {}
        correct = {'strand1': "TGTACCCGCC GATCCCCGACTA A AAACTC TGGGTATTGGGGTGTACTTCCACCAA",
                   'strand2': "CG ACTCACAACATTCG GA TAGAGAAAGCCGTTAG ACAGGGCT TAGTGAGACATT",
                   'score': -10}
        s1 = "TGTACCCGCCGATCCCCGACTAAAAACTCTGGGTATTGGGGTGTACTTCCACCAA"
        s2 = "CGACTCACAACATTCGGATAGAGAAAGCCGTTAGACAGGGCTTAGTGAGACATT"
        test = findOptimalAlignment(s1, s2, cache)
        self.assertDictEqual(correct, test)


def main():
    cache = {}
    #unittest.main()  # uncomment to run the test suite
    while (True):
        sys.stdout.write("Generate random DNA strands? ")
        answer = sys.stdin.readline()
        if answer == "no\n":
            break
        strand1 = generateRandomDNAStrand(50, 60)
        strand2 = generateRandomDNAStrand(50, 60)
        sys.stdout.write("Aligning these two strands: \n\n\t" + strand1 + "\n")
        sys.stdout.write("\t" + strand2 + "\n")
        alignment = findOptimalAlignment(strand1, strand2, cache)
        printAlignment(alignment)

if __name__ == "__main__":
    main()
