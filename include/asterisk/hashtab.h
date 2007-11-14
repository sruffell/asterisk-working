/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 2006, Digium, Inc.
 *
 * Steve Murphy <murf@digium.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */
#ifndef _ASTERISK_HASHTAB_H_
#define _ASTERISK_HASHTAB_H_
#define __USE_UNIX98 1          /* to get the MUTEX_RECURSIVE stuff */

/*! \file
 * \brief Generic (perhaps overly so) hashtable implementation
*/
/*! \note

A hash table is a structure that allows for an exact-match search
in O(1) (or close to that) time.

The method: given: a set of {key,val} pairs. (at a minimum).
            given: a hash function, which, given a key,
            will return an integer. Ideally, each key in the
            set will have its own unique associated hash value.
			This hash number will index into an array. "buckets"
            are what the elements of this array are called. To
            handle possible collisions in hash values, buckets can form a list.

The key for a value must be contained in the value, or we won't
be able to find it in the bucket list.

This implementation is pretty generic, because:
 1. The value and key are expected to be in a structure
    (along with other data, perhaps) and it's address is a "void *".
 2. The pointer to a compare function must be passed in at the
    time of creation, and is stored in the hashtable.
 3. The pointer to a resize function, which returns 1 if the
    hash table is to be grown. A default routine is provided
    if the pointer is NULL, and uses the java hashtable metric
    of a 75% load factor.
 4. The pointer to a "new size" function, which returns a preferable
    new size for the hash table bucket array. By default, a function
    is supplied which roughly doubles the size of the array, is provided.
    This size should ideally be a prime number.
 5. The hashing function pointer must also be supplied. This function
    must be written by the user to access the keys in the objects being
    stored. Some helper functions that use a simple "mult by prime, add
    the next char", sort of string hash, or a simple modulus of the hash
    table size for ints, is provided; the user can use these simple
    algorithms to generate a hash, or implement any other algorithms they
    wish.
 6. Recently updated the hash routines to use Doubly-linked lists for buckets,
    and added a doubly-linked list that threads thru every bucket in the table.
    The list of all buckets is on the HashTab struct. The Traversal was modified
    to go thru this list instead of searching the bucket array for buckets.
    This also should make it safe to remove a bucket during the traversal.
    Removal and destruction routines will work faster.
*/

struct ast_hashtab_bucket
{
	const void *object;                    /*!< whatever it is we are storing in this table */
	struct ast_hashtab_bucket *next;       /*!< a DLL of buckets in hash collision */
	struct ast_hashtab_bucket *prev;       /*!< a DLL of buckets in hash collision */
	struct ast_hashtab_bucket *tnext;      /*!< a DLL of all the hash buckets for traversal */
	struct ast_hashtab_bucket *tprev;      /*!< a DLL of all the hash buckets for traversal */
};

struct ast_hashtab
{
	struct ast_hashtab_bucket **array;
	struct ast_hashtab_bucket *tlist;		/*!< the head of a DLList of all the hashbuckets in the table (for traversal). */
	
	int (*compare) (const void *a, const void *b);	/*!< a ptr to func that returns int, and take two void* ptrs, compares them, 
													 rets -1 if a < b; rets 0 if a==b; rets 1 if a>b */
	int (*newsize) (struct ast_hashtab *tab);	/*!< a ptr to func that returns int, a new size for hash tab, based on curr_size */
	int (*resize) (struct ast_hashtab *tab);	/*!< a function to decide whether this hashtable should be resized now */
	unsigned int (*hash) (const void *obj);         /*!< a hash func ptr for this table. Given a raw ptr to an obj, 
													 it calcs a hash.*/
	int hash_tab_size;                            /*!< the size of the bucket array */
	int hash_tab_elements;                        /*!< the number of objects currently stored in the table */
	int largest_bucket_size;                      /*!< a stat on the health of the table */
	int resize_count;                             /*!< a count of the number of times this table has been
													 resized */
	int do_locking;                               /*!< if 1 use locks to guarantee safety of insertions/deletions */
	/* this spot reserved for the proper lock storage */
	ast_rwlock_t lock;                                /* is this as good as it sounds? */
};

/*! \brief an iterator for traversing the buckets */
struct ast_hashtab_iter
{
	struct ast_hashtab *tab;
	struct ast_hashtab_bucket *next;
};


/* some standard, default routines for general use */

/*! \brief For sizing the hash table, tells if num is prime or not */
int ast_is_prime(int num);

/*! 
 * \brief assumes a and b are char * 
 * \return 0 if they match 
*/
int ast_hashtab_compare_strings(const void *a, const void *b);

/*!
 * \brief assumes a & b are strings
 * \return 0 if they match (strcasecmp) 
*/
int ast_hashtab_compare_strings_nocase(const void *a, const void *b);

/*!
 * \brief assumes a & b are int *
 * \retval 0 if match
 * \retval 1 a > b
 * \retval -1 a < b
*/
int ast_hashtab_compare_ints(const void *a, const void *b);

/*!
 * \brief assumes a & b are short *
 * \retval 0 if match
 * \retval 1 a > b
 * \retval -1 a < b
*/
int ast_hashtab_compare_shorts(const void *a, const void *b);

/*!
 * \brief determine if resize should occur
 * \returns 1 if the table is 75% full or more
*/
int ast_hashtab_resize_java(struct ast_hashtab *tab);

/*! \brief no resizing; always return 0 */
int ast_hashtab_resize_tight(struct ast_hashtab *tab);

/*! \brief no resizing; always return 0 */
int ast_hashtab_resize_none(struct ast_hashtab *tab);

/*! \brief Create a prime number roughly 2x the current table size */
int ast_hashtab_newsize_java(struct ast_hashtab *tab);

/* not yet specified, probably will return 1.5x the current table size */
int ast_hashtab_newsize_tight(struct ast_hashtab *tab);

/*! \brief always return current size -- no resizing */
int ast_hashtab_newsize_none(struct ast_hashtab *tab);

/*! 
 * \brief Hashes a string to a number
 * \param obj
 * \note A modulus is applied so it in the range 0 to mod-1 
*/
unsigned int ast_hashtab_hash_string(const void *obj);

/*! \brief Upperases each char before using them for a hash */
unsigned int ast_hashtab_hash_string_nocase(const void *obj);


unsigned int ast_hashtab_hash_string_sax(const void *obj); /* from Josh */


unsigned int ast_hashtab_hash_int(const int num);  /* right now, both these funcs are just result = num%modulus; */


unsigned int ast_hashtab_hash_short(const short num);


/*!
 * \brief Create the hashtable list
 * \param initial_buckets starting number of buckets
 * \param compare a func ptr to compare two elements in the hash -- cannot be null 
 * \param resize a func ptr to decide if the table needs to be resized, a NULL ptr here will cause a default to be used
 * \param newsize a func ptr that returns a new size of the array. A NULL will cause a default to be used
 * \param hash a func ptr to do the hashing
 * \param do_locking use locks to guarantee safety of iterators/insertion/deletion -- real simpleminded right now
*/
struct ast_hashtab * ast_hashtab_create(int initial_buckets,
					int (*compare)(const void *a, const void *b), 
					int (*resize)(struct ast_hashtab *),	
					int (*newsize)(struct ast_hashtab *tab),
					unsigned int (*hash)(const void *obj), 
					int do_locking );

/*!
 * \brief This func will free the hash table and all its memory. 
 * \note It doesn't touch the objects stored in it
 * \param tab
 * \param objdestroyfunc
*/
void ast_hashtab_destroy( struct ast_hashtab *tab, void (*objdestroyfunc)(void *obj));


/*!
 * \brief Insert without checking 
 * \param tab
 * \param obj
 *
 * Normally, you'd insert "safely" by checking to see if the element is
 * already there; in this case, you must already have checked. If an element
 * is already in the hashtable, that matches this one, most likely this one
 * will be found first. 
 * \note will force a resize if the resize func returns 1
 * \retval 1 on success
 * \retval 0 if there's a problem
*/
int ast_hashtab_insert_immediate(struct ast_hashtab *tab, const void *obj);

/*!
 * \brief Insert without checking, hashing or locking
 * \param tab
 * \param obj
 * \param h hashed index value
 * 
 * \note Will force a resize if the resize func returns 1
 * \retval 1 on success
 * \retval 0 if there's a problem
*/
int ast_hashtab_insert_immediate_bucket(struct ast_hashtab *tab, const void *obj, unsigned int h);

/*!
 * \brief Check and insert new object only if it is not there.
 * \note Will force a resize if the resize func returns 1
 * \retval 1 on success
 * \retval  0 if there's a problem, or it's already there. 
*/
int ast_hashtab_insert_safe(struct ast_hashtab *tab, const void *obj);

/*!
 * \brief Lookup this object in the hash table. 
 * \param tab 
 * \param obj 
 * \retval a ptr if found
 * \retval NULL if not found
*/
void * ast_hashtab_lookup(struct ast_hashtab *tab, const void *obj);

/*!
 * \brief  Use this if have the hash val for the object
 * \note This and avoid the recalc of the hash (the modulus (table_size) is not applied)
*/
void * ast_hashtab_lookup_with_hash(struct ast_hashtab *tab, const void *obj, unsigned int hashval);

/*!
 * \brief Similar to ast_hashtab_lookup but sets h to the key hash value if the lookup fails.
 * \note This has the modulus applied, and will not be useful for long term storage if the table is resizable.
*/
void * ast_hashtab_lookup_bucket(struct ast_hashtab *tab, const void *obj, unsigned int *h);

/*! \brief Returns key stats for the table */
void ast_hashtab_get_stats( struct ast_hashtab *tab, int *biggest_bucket_size, int *resize_count, int *num_objects, int *num_buckets);

/*! \brief Returns the number of elements stored in the hashtab */
int ast_hashtab_size( struct ast_hashtab *tab);

/*! \brief Returns the size of the bucket array in the hashtab */
int ast_hashtab_capacity( struct ast_hashtab *tab);

/*! \brief Return a copy of the hash table */
struct ast_hashtab *ast_hashtab_dup(struct ast_hashtab *tab, void *(*obj_dup_func)(const void *obj));

/*! \brief Gives an iterator to hastable */
struct ast_hashtab_iter *ast_hashtab_start_traversal(struct ast_hashtab *tab);

/*! \brief end the traversal, free the iterator, unlock if necc. */
void ast_hashtab_end_traversal(struct ast_hashtab_iter *it);

/*! \brief Gets the next object in the list, advances iter one step returns null on end of traversal */
void *ast_hashtab_next(struct ast_hashtab_iter *it);

/*! \brief Looks up the object, removes the corresponding bucket */
void *ast_hashtab_remove_object_via_lookup(struct ast_hashtab *tab, void *obj);

/*! \brief Hash the object and then compare ptrs in bucket list instead of
	   calling the compare routine, will remove the bucket */
void *ast_hashtab_remove_this_object(struct ast_hashtab *tab, void *obj);

/* ------------------ */
/* for lock-enabled traversals with ability to remove an object during the traversal*/
/* ------------------ */

/*! \brief Gives an iterator to hastable */
struct ast_hashtab_iter *ast_hashtab_start_write_traversal(struct ast_hashtab *tab);

/*! \brief Looks up the object, removes the corresponding bucket */
void *ast_hashtab_remove_object_via_lookup_nolock(struct ast_hashtab *tab, void *obj);

/*! \brief Hash the object and then compare ptrs in bucket list instead of
	   calling the compare routine, will remove the bucket */
void *ast_hashtab_remove_this_object_nolock(struct ast_hashtab *tab, void *obj);

/* ------------------ */
/* ------------------ */

/* user-controlled hashtab locking. Create a hashtab without locking, then call the
   following locking routines yourself to lock the table between threads. */

/*! \brief Call this after you create the table to init the lock */
void ast_hashtab_initlock(struct ast_hashtab *tab); 
/*! \brief Request a write-lock on the table. */
void ast_hashtab_wrlock(struct ast_hashtab *tab); 
/*! \brief Request a read-lock on the table -- don't change anything! */
void ast_hashtab_rdlock(struct ast_hashtab *tab); 
/*! \brief release a read- or write- lock. */
void ast_hashtab_unlock(struct ast_hashtab *tab); 
/*! \brief Call this before you destroy the table. */
void ast_hashtab_destroylock(struct ast_hashtab *tab); 


#endif
