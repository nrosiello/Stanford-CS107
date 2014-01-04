#!/usr/bin/env python

from __future__ import print_function
import random  # for seed, random
import sys    # for stdout


# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2):
    # if one of the two strands is empty, then there is only
    # one possible alignment, and of course it's optimal
    if len(strand1) == 0:
        return {'strand1': ' ' * len(strand2),
                'strand2': strand2,
                'score': len(strand2) * -2}
    if len(strand2) == 0:
        return {'strand1': strand1,
                'strand2': ' ' * len(strand1),
                'score': len(strand1) * -2}

    # There's the scenario where the two leading bases of
    # each strand are forced to align, regardless of whether or not
    # they actually match.
    bestWith = findOptimalAlignment(strand1[1:], strand2[1:])
    if strand1[0] == strand2[0]:
        # no benefit from making other recursive calls
        return {'strand1': strand1[0] + bestWith['strand1'],
                'strand2': strand2[0] + bestWith['strand2'],
                'score': bestWith['score'] + 1}

    best = {'strand1': strand1.ljust(len(strand2)),
            'strand2': strand2.ljust(len(strand1)),
            'score': bestWith['score'] - 1}

    # It's possible that the leading base of strand1 best
    # matches not the leading base of strand2, but the one after it.
    bestWithout = findOptimalAlignment(strand1, strand2[1:])
    bestWithout['score'] -= 2  # penalize for insertion of space
    if bestWithout['score'] > best['score']:
        bestWithout['strand1'] = ' ' + bestWithout['strand1']
        bestWithout['strand2'] = strand2[0] + bestWithout['strand2']
        best = bestWithout

    # opposite scenario
    bestWithout = findOptimalAlignment(strand1[1:], strand2)
    bestWithout['score'] -= 2  # penalize for insertion of space
    if bestWithout['score'] > best['score']:
        bestWithout['strand2'] = ' ' + bestWithout['strand2']
        bestWithout['strand1'] = strand1[0] + bestWithout['strand1']
        best = bestWithout

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


def printAlignment(score, out=sys.stdout):
    out.write("Optimal alignment score is " + str(score) + "\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.


def main():
    while (True):
        sys.stdout.write("Generate random DNA strands? ")
        answer = sys.stdin.readline()
        if answer == "no\n":
            break
        strand1 = generateRandomDNAStrand(8, 10)
        strand2 = generateRandomDNAStrand(8, 10)
        sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
        sys.stdout.write("                            " + strand2 + "\n")
        alignment = findOptimalAlignment(strand1, strand2)
        printAlignment(alignment)

if __name__ == "__main__":
    main()
