;; The Where-Am-I helper functions:
;; originally written by Nick Parlante

;;
;;  the 2-D POINT type  (x and y coordinates)
;;  impelemented as a list length 2
;;
;;  the CIRCLE type (a radius and a center point)
;;  implemented as a list length 2-- first element is the radius and
;;  the second is the center point
;;
;;  For convenience, I do not insist that you treat these as ADT's-
;;  so if you want to use CAR to get the x coordinate, or build you own
;;  circles without going through MAKE-CIRCLE that will be ok.
;; 
;; POINT functions:
;;  make-pt   create a new point
;;  x         get the x coordinate of a point
;;  y         get the y coordinate of a point
;;  dist      return the distance between two points
;;
;; CIRCLE functions
;;  make-circle   create a new circle
;;  radius        get the radius of a circle
;;  center        get the center of a circle
;;  intersect     given two circles, returns a list of the points
;;                of intersection for those circles.
;;                For the purposes of this program I have
;;                bastardized the definition of 'intersect' a little
;;                to give better results when the measurements are
;;                inexact.  Don't worry about that, just use the points
;;                returned.  Someone who is interested in Math or Geometry
;;                might be interested to see how I compute the intersection.

(define (make-pt x y) 
  (list x y))

(define (x pt) 
  (car pt))

(define (y pt) 
  (cadr pt))

(define (dist pt1 pt2) 
  (let ((dx (- (x pt1) (x pt2)))
        (dy (- (y pt1) (y pt2))))
    (sqrt (+ (* dx dx) (* dy dy)))))

;;
;; 2D vector operations - used by the intersection function
;; vectors are a lot like points.  You won't need these.
;; 

(define (add v1 v2)
  (list (+ (car v1) (car v2))
	(+ (cadr v1) (cadr v2))))

(define (sub v1 v2)
  (list (- (car v1) (car v2))
	(- (cadr v1) (cadr v2))))

(define (len v)
  (sqrt (+ (* (car v) (car v))
	   (* (cadr v) (cadr v)))))

(define (scale v factor)
  (list (* (car v) factor) 
	(* (cadr v) factor)))

(define (normalize v)
  (scale (list (- (cadr v)) (car v)) (/ (len v))))


(define (make-circle radius center) 
  (list radius center))

(define (radius circle) 
  (car circle))

(define (center circle) 
  (cadr circle))

;;
;; Function: intersect
;; -------------------
;; Return a list of the points of intersection of the two circles.
;; The circles may not have the same center point
;;

(define (intersect circle1 circle2)
  (if (equal? (center circle1) (center circle2))
      (error "Intersect cannont handle circles with the same center point.")
      (let* ((c1 (center circle1))
	     (r1 (radius circle1))
	     (c2 (center circle2))
	     (r2 (radius circle2))
	     (d (dist c1 c2)))
      ;; first check to see if the circles are too far apart to intersect,
      ;; or if one circle is within another.
	(if (or (> d (+ r1 r2)) (> r1 (+ d r2)) (> r2 (+ d r1)))
	    ;; if there is no real intersection, use the closest tangent points on each
	    ;; circle.  This is the bastardization above.
	    (list (add c1 (scale (sub c2 c1) (/ r1 d)))  ;; c1-> towards c2
		  (add c2 (scale (sub c1 c2) (/ r2 d)))) ;; c2-> towards c1
    ;;otherwise the circles intersect normally, and I did some hairy
    ;;geometry to show that the following computes the two points
    ;;of intersection.
    (let* ((r12 (* r1 r1))
           (r22 (* r2 r2))
           (d2 (* d d))
           (d1 (/ (+ r12 (- r22) d2) 2 d))
           (h (sqrt (- r12 (* d1 d1))))
           (towards (scale (sub c2 c1) (/ d1 d))) ;;vector c1->c2
           (perp (scale (normalize towards) h)))
      (list (add c1 (add towards perp))
            (add c1 (add towards (scale perp -1)))))))))

;;
;; Function: prefix-of-list
;; ------------------------
;; Accepts the incoming list and returns one
;; with the same first k elements and nothing more.
;;

(define (prefix-of-list ls k)
  (if (or (zero? k) (null? ls)) '()
      (cons (car ls) (prefix-of-list (cdr ls) (- k 1)))))

;;
;; Function: partition
;; -------------------
;; Takes a pivot and a list and produces a pair two lists.
;; The first of the two lists contains all of those element less than the 
;; pivot, and the second contains everything else.  Notice that
;; the first list pair every produced is (() ()), and as the
;; recursion unwinds exactly one of the two lists gets a new element
;; cons'ed to the front of it.  
;; 

(define (partition pivot num-list cmp)
  (if (null? num-list) '(() ())
      (let ((split-of-rest (partition pivot (cdr num-list) cmp)))
	(if (cmp (car num-list) pivot)
	    (list (cons (car num-list) (car split-of-rest)) (cadr split-of-rest))
	    (list (car split-of-rest) (cons (car num-list) (car (cdr split-of-rest))))))))

;;
;; Function: quicksort
;; -------------------
;; Implements the quicksort algorithm to sort lists of numbers from
;; high to low.  If a list is of length 0 or 1, then it is trivially
;; sorted.  Otherwise, we partition to cdr of the list around the car
;; to generate two lists: those in the cdr that are smaller than the car,
;; and those in the cdr that are greater than or equal to the car.  
;; We then recursively quicksort the two lists, and then splice everything
;; together in the proper order.
;;

(define (quicksort num-list cmp)
  (if (<= (length num-list) 1) num-list
      (let ((split (partition (car num-list) (cdr num-list) cmp)))
	(append (quicksort (car split) cmp) 
		(list (car num-list)) 
		(quicksort (cadr split) cmp)))))

(newline)
(display "Test quicksort")
(newline)
(display (quicksort '(3 4 1 6 5 2) >=))
(newline)

;;
;; Function: remove
;; ----------------
;; Generates a copy of the specified list, except that all
;; instances that match the specified elem in the equal? sense
;; are excluded.
;;

(define (remove elem ls)
  (cond ((null? ls) '())
	((equal? (car ls) elem) (remove elem (cdr ls)))
	(else (cons (car ls) (remove elem (cdr ls))))))
                  
;; 
;; Function: all-guesses
;; ---------------------
;; Given a list of distances and a list of stars, return a list of all
;; the possible guesses.  A single guess is a list of circles which pairs
;; each distance with one of the stars.
;; 

(define (all-guesses distances stars)
  (if (or (null? distances) (null? stars)) '(())
      (apply append 
	     (map (lambda (star)
		    (map (lambda (pair) 
			   (cons (list (car distances) star) pair))
			 (all-guesses (cdr distances) (remove star stars))
			 )
		    )
		  stars))))

(define *distances-1* '(2.65 5.55 5.25))
(define *stars-1* '((0 0) (4 6) (10 0) (7 4) (12 5)))

(define *distances-2* '(2.5 11.65 7.75))
(define *stars-2* '((0 0) (4 4) (10 0)))

;; 
;; Function: intersection-points
;; -----------------------------
;; Given a list of circles, returns a list of all points where the circles
;; intersect.  Each circle in the list may intersect with all any other
;; circle in the list.
;;

(define (intersection-points points)
  (define (intersections-with-first-point)
    (apply append (map (lambda (pt) 
                         (intersect (car points) pt)) 
                       (cdr points))))
  (if (< (length points) 2)
    '()
    (append (intersections-with-first-point) 
            (intersection-points (cdr points)))))

(newline)
(display "Test intersection-points")
(newline)
(display (intersection-points '((1 (0 0)) (1 (1 0)))))
(newline)
(display (intersection-points '((1 (0 0)) (1 (1 0)) (1 (1 1)))))
(newline)

;;
;; Function: distance-product
;; --------------------------
;; Given a point and a list of points, returns the product of the distances 
;; between that point and all points in the list.  If the point is in the list,
;; it should be removed so that it does not force the product to be zero.
;;

(define (distance-product pt1 ls)
  (let ((ls-without-pt1 (remove pt1 ls)))
    (apply * (map (lambda (pt2) (dist pt1 pt2)) ls-without-pt1))))

(newline)
(display "Test distance-product")
(newline)
(display (distance-product '(2 0) '((0 0) (2 0) (6 0))))
(newline)
(display (distance-product '(3 3) '((2 5) (7 8) (10 1) (3 2))))
(newline)

;; 
;; Function: rate-points
;; ---------------------
;; Given a list of points, annotates each point in the list with the
;; distance-product from the other points.
;;

(define (rate-points points)
  (map (lambda (pt) (list (distance-product pt points) pt)) points))

(newline)
(display "Test rate-points")
(newline)
(display (rate-points '((0 0) (2 0) (6 0))))
(newline)
(display (rate-points '((2 5) (7 8) (10 1) (3 2))))
(newline)

;; 
;; Function: sort-points
;; ---------------------
;; Given a list of rated points, sorts them in ascending order of rating.
;;

(define (sort-points points)
  (quicksort points (lambda (pt1 pt2) (< (car pt1) (car pt2)))))

(newline)
(display "Test sort-points")
(newline)
(display (sort-points (rate-points '((2 5) (7 8) (10 1) (3 2)))))
(newline)
(display (sort-points (rate-points '((0 0) (2 0) (6 0)))))
(newline)

;; 
;; Function: clumped-points
;; ------------------------
;; Given a list of points, rates and sorts them based on distance to other points,
;; then returns the half of the points with the smallest ratings.
;;

(define (clumped-points points)
  (let ((sorted-pts (map cadr (sort-points (rate-points points)))))
    (prefix-of-list sorted-pts (floor (/ (length sorted-pts) 2))))) 
  

(newline)
(display "Test clumped-points")
(newline)
(display (clumped-points '((0 0) (2 0) (6 0))))
(newline)
(display (clumped-points '((0 0) (2 0) (6 0) (1 0))))
(newline)

;; 
;; Function: average-point
;; -----------------------
;; Given a list of points, computes a point that is the average of the x and y
;; values.  Also includes the distance rating of how far the average point is
;; from all the points.
;;

(define (average ls)
  (/ (apply + ls) (length ls)))

(define (average-point points)
  (let ((avg-pt (list (average (map car points))
                      (average (map cadr points)))))
    (list (distance-product avg-pt points) avg-pt)))

(newline)
(display "Test average-point")
(newline)
(display (average-point '((0 0) (2 0) (6 0))))
(newline)
(display (average-point '((0 0) (2 0) (6 0) (1 0) (5 4) (4 5))))
(newline)

;; 
;; Function: best-estimate
;; -----------------------
;; Given a list of circles representing a guess, reduces those points down to the
;; most clumped, then returns their average point.
;;

(define (best-estimate circles)
  (average-point (clumped-points (intersection-points circles))))

(newline)
(display "Test best-estimate")
(newline)
(display (best-estimate '((1 (0 0)) (1 (2 0)) (0.1 (1 0)))))
(newline)

;; 
;; Function: where-am-i
;; --------------------
;; Given a list of distances and star locations, computes all possible guesses for
;; your location.  The estimates are sorted in increasing order of distance
;; rating.
;;

(define (where-am-i distances stars)
  (quicksort (map best-estimate (all-guesses distances stars)) 
             (lambda (pt1 pt2) (< (car pt1) (car pt2)))))

(newline)
(display "Test where-am-i")
(newline)
(display (where-am-i '(2.5 11.65 7.75) '((0 0) (4 4) (10 0))))
(newline)
(display (where-am-i '(1 2) '((0 0) (3 0))))
(newline)
(display (where-am-i '(2.65 5.55 5.25) '((0 0) (4 6) (10 0) (7 4) (12 5))))
(newline)
