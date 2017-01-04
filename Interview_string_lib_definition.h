http://lxr.free-electrons.com/source/lib/string.c
 1 /*
  2  *  linux/lib/string.c
  3  *
  4  *  Copyright (C) 1991, 1992  Linus Torvalds
  5  */
  6 
  7 /*
  8  * stupid library routines.. The optimized versions should generally be found
  9  * as inline code in <asm-xx/string.h>
 10  *
 11  * These are buggy as well..
 12  *
 13  * * Fri Jun 25 1999, Ingo Oeser <ioe@informatik.tu-chemnitz.de>
 14  * -  Added strsep() which will replace strtok() soon (because strsep() is
 15  *    reentrant and should be faster). Use only strsep() in new code, please.
 16  *
 17  * * Sat Feb 09 2002, Jason Thomas <jason@topic.com.au>,
 18  *                    Matthew Hawkins <matt@mh.dropbear.id.au>
 19  * -  Kissed strtok() goodbye
 20  */
 21 
 22 #include <linux/types.h>
 23 #include <linux/string.h>
 24 #include <linux/ctype.h>
 25 #include <linux/kernel.h>
 26 #include <linux/export.h>
 27 #include <linux/bug.h>
 28 #include <linux/errno.h>
 29 
 30 #include <asm/byteorder.h>
 31 #include <asm/word-at-a-time.h>
 32 #include <asm/page.h>
 33 
 34 #ifndef __HAVE_ARCH_STRNCASECMP
 35 /**
 36  * strncasecmp - Case insensitive, length-limited string comparison
 37  * @s1: One string
 38  * @s2: The other string
 39  * @len: the maximum number of characters to compare
 40  */
 41 int strncasecmp(const char *s1, const char *s2, size_t len)
 42 {
 43         /* Yes, Virginia, it had better be unsigned */
 44         unsigned char c1, c2;
 45 
 46         if (!len)
 47                 return 0;
 48 
 49         do {
 50                 c1 = *s1++;
 51                 c2 = *s2++;
 52                 if (!c1 || !c2)
 53                         break;
 54                 if (c1 == c2)
 55                         continue;
 56                 c1 = tolower(c1);
 57                 c2 = tolower(c2);
 58                 if (c1 != c2)
 59                         break;
 60         } while (--len);
 61         return (int)c1 - (int)c2;
 62 }
 63 EXPORT_SYMBOL(strncasecmp);
 64 #endif
 65 
 66 #ifndef __HAVE_ARCH_STRCASECMP
 67 int strcasecmp(const char *s1, const char *s2)
 68 {
 69         int c1, c2;
 70 
 71         do {
 72                 c1 = tolower(*s1++);
 73                 c2 = tolower(*s2++);
 74         } while (c1 == c2 && c1 != 0);
 75         return c1 - c2;
 76 }
 77 EXPORT_SYMBOL(strcasecmp);
 78 #endif
 79 
 80 #ifndef __HAVE_ARCH_STRCPY
 81 /**
 82  * strcpy - Copy a %NUL terminated string
 83  * @dest: Where to copy the string to
 84  * @src: Where to copy the string from
 85  */
 86 #undef strcpy
 87 char *strcpy(char *dest, const char *src)
 88 {
 89         char *tmp = dest;
 90 
 91         while ((*dest++ = *src++) != '\0')
 92                 /* nothing */;
 93         return tmp;
 94 }
 95 EXPORT_SYMBOL(strcpy);
 96 #endif
 97 
 98 #ifndef __HAVE_ARCH_STRNCPY
 99 /**
100  * strncpy - Copy a length-limited, C-string
101  * @dest: Where to copy the string to
102  * @src: Where to copy the string from
103  * @count: The maximum number of bytes to copy
104  *
105  * The result is not %NUL-terminated if the source exceeds
106  * @count bytes.
107  *
108  * In the case where the length of @src is less than  that  of
109  * count, the remainder of @dest will be padded with %NUL.
110  *
111  */
112 char *strncpy(char *dest, const char *src, size_t count)
113 {
114         char *tmp = dest;
115 
116         while (count) {
117                 if ((*tmp = *src) != 0)
118                         src++;
119                 tmp++;
120                 count--;
121         }
122         return dest;
123 }
124 EXPORT_SYMBOL(strncpy);
125 #endif
126 
127 #ifndef __HAVE_ARCH_STRLCPY
128 /**
129  * strlcpy - Copy a C-string into a sized buffer
130  * @dest: Where to copy the string to
131  * @src: Where to copy the string from
132  * @size: size of destination buffer
133  *
134  * Compatible with *BSD: the result is always a valid
135  * NUL-terminated string that fits in the buffer (unless,
136  * of course, the buffer size is zero). It does not pad
137  * out the result like strncpy() does.
138  */
139 size_t strlcpy(char *dest, const char *src, size_t size)
140 {
141         size_t ret = strlen(src);
142 
143         if (size) {
144                 size_t len = (ret >= size) ? size - 1 : ret;
145                 memcpy(dest, src, len);
146                 dest[len] = '\0';
147         }
148         return ret;
149 }
150 EXPORT_SYMBOL(strlcpy);
151 #endif
152 
153 #ifndef __HAVE_ARCH_STRSCPY
154 /**
155  * strscpy - Copy a C-string into a sized buffer
156  * @dest: Where to copy the string to
157  * @src: Where to copy the string from
158  * @count: Size of destination buffer
159  *
160  * Copy the string, or as much of it as fits, into the dest buffer.
161  * The routine returns the number of characters copied (not including
162  * the trailing NUL) or -E2BIG if the destination buffer wasn't big enough.
163  * The behavior is undefined if the string buffers overlap.
164  * The destination buffer is always NUL terminated, unless it's zero-sized.
165  *
166  * Preferred to strlcpy() since the API doesn't require reading memory
167  * from the src string beyond the specified "count" bytes, and since
168  * the return value is easier to error-check than strlcpy()'s.
169  * In addition, the implementation is robust to the string changing out
170  * from underneath it, unlike the current strlcpy() implementation.
171  *
172  * Preferred to strncpy() since it always returns a valid string, and
173  * doesn't unnecessarily force the tail of the destination buffer to be
174  * zeroed.  If the zeroing is desired, it's likely cleaner to use strscpy()
175  * with an overflow test, then just memset() the tail of the dest buffer.
176  */
177 ssize_t strscpy(char *dest, const char *src, size_t count)
178 {
179         const struct word_at_a_time constants = WORD_AT_A_TIME_CONSTANTS;
180         size_t max = count;
181         long res = 0;
182 
183         if (count == 0)
184                 return -E2BIG;
185 
186 #ifdef CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS
187         /*
188          * If src is unaligned, don't cross a page boundary,
189          * since we don't know if the next page is mapped.
190          */
191         if ((long)src & (sizeof(long) - 1)) {
192                 size_t limit = PAGE_SIZE - ((long)src & (PAGE_SIZE - 1));
193                 if (limit < max)
194                         max = limit;
195         }
196 #else
197         /* If src or dest is unaligned, don't do word-at-a-time. */
198         if (((long) dest | (long) src) & (sizeof(long) - 1))
199                 max = 0;
200 #endif
201 
202         while (max >= sizeof(unsigned long)) {
203                 unsigned long c, data;
204 
205                 c = *(unsigned long *)(src+res);
206                 if (has_zero(c, &data, &constants)) {
207                         data = prep_zero_mask(c, data, &constants);
208                         data = create_zero_mask(data);
209                         *(unsigned long *)(dest+res) = c & zero_bytemask(data);
210                         return res + find_zero(data);
211                 }
212                 *(unsigned long *)(dest+res) = c;
213                 res += sizeof(unsigned long);
214                 count -= sizeof(unsigned long);
215                 max -= sizeof(unsigned long);
216         }
217 
218         while (count) {
219                 char c;
220 
221                 c = src[res];
222                 dest[res] = c;
223                 if (!c)
224                         return res;
225                 res++;
226                 count--;
227         }
228 
229         /* Hit buffer length without finding a NUL; force NUL-termination. */
230         if (res)
231                 dest[res-1] = '\0';
232 
233         return -E2BIG;
234 }
235 EXPORT_SYMBOL(strscpy);
236 #endif
237 
238 #ifndef __HAVE_ARCH_STRCAT
239 /**
240  * strcat - Append one %NUL-terminated string to another
241  * @dest: The string to be appended to
242  * @src: The string to append to it
243  */
244 #undef strcat
245 char *strcat(char *dest, const char *src)
246 {
247         char *tmp = dest;
248 
249         while (*dest)
250                 dest++;
251         while ((*dest++ = *src++) != '\0')
252                 ;
253         return tmp;
254 }
255 EXPORT_SYMBOL(strcat);
256 #endif
257 
258 #ifndef __HAVE_ARCH_STRNCAT
259 /**
260  * strncat - Append a length-limited, C-string to another
261  * @dest: The string to be appended to
262  * @src: The string to append to it
263  * @count: The maximum numbers of bytes to copy
264  *
265  * Note that in contrast to strncpy(), strncat() ensures the result is
266  * terminated.
267  */
268 char *strncat(char *dest, const char *src, size_t count)
269 {
270         char *tmp = dest;
271 
272         if (count) {
273                 while (*dest)
274                         dest++;
275                 while ((*dest++ = *src++) != 0) {
276                         if (--count == 0) {
277                                 *dest = '\0';
278                                 break;
279                         }
280                 }
281         }
282         return tmp;
283 }
284 EXPORT_SYMBOL(strncat);
285 #endif
286 
287 #ifndef __HAVE_ARCH_STRLCAT
288 /**
289  * strlcat - Append a length-limited, C-string to another
290  * @dest: The string to be appended to
291  * @src: The string to append to it
292  * @count: The size of the destination buffer.
293  */
294 size_t strlcat(char *dest, const char *src, size_t count)
295 {
296         size_t dsize = strlen(dest);
297         size_t len = strlen(src);
298         size_t res = dsize + len;
299 
300         /* This would be a bug */
301         BUG_ON(dsize >= count);
302 
303         dest += dsize;
304         count -= dsize;
305         if (len >= count)
306                 len = count-1;
307         memcpy(dest, src, len);
308         dest[len] = 0;
309         return res;
310 }
311 EXPORT_SYMBOL(strlcat);
312 #endif
313 
314 #ifndef __HAVE_ARCH_STRCMP
315 /**
316  * strcmp - Compare two strings
317  * @cs: One string
318  * @ct: Another string
319  */
320 #undef strcmp
321 int strcmp(const char *cs, const char *ct)
322 {
323         unsigned char c1, c2;
324 
325         while (1) {
326                 c1 = *cs++;
327                 c2 = *ct++;
328                 if (c1 != c2)
329                         return c1 < c2 ? -1 : 1;
330                 if (!c1)
331                         break;
332         }
333         return 0;
334 }
335 EXPORT_SYMBOL(strcmp);
336 #endif
337 
338 #ifndef __HAVE_ARCH_STRNCMP
339 /**
340  * strncmp - Compare two length-limited strings
341  * @cs: One string
342  * @ct: Another string
343  * @count: The maximum number of bytes to compare
344  */
345 int strncmp(const char *cs, const char *ct, size_t count)
346 {
347         unsigned char c1, c2;
348 
349         while (count) {
350                 c1 = *cs++;
351                 c2 = *ct++;
352                 if (c1 != c2)
353                         return c1 < c2 ? -1 : 1;
354                 if (!c1)
355                         break;
356                 count--;
357         }
358         return 0;
359 }
360 EXPORT_SYMBOL(strncmp);
361 #endif
362 
363 #ifndef __HAVE_ARCH_STRCHR
364 /**
365  * strchr - Find the first occurrence of a character in a string
366  * @s: The string to be searched
367  * @c: The character to search for
368  */
369 char *strchr(const char *s, int c)
370 {
371         for (; *s != (char)c; ++s)
372                 if (*s == '\0')
373                         return NULL;
374         return (char *)s;
375 }
376 EXPORT_SYMBOL(strchr);
377 #endif
378 
379 #ifndef __HAVE_ARCH_STRCHRNUL
380 /**
381  * strchrnul - Find and return a character in a string, or end of string
382  * @s: The string to be searched
383  * @c: The character to search for
384  *
385  * Returns pointer to first occurrence of 'c' in s. If c is not found, then
386  * return a pointer to the null byte at the end of s.
387  */
388 char *strchrnul(const char *s, int c)
389 {
390         while (*s && *s != (char)c)
391                 s++;
392         return (char *)s;
393 }
394 EXPORT_SYMBOL(strchrnul);
395 #endif
396 
397 #ifndef __HAVE_ARCH_STRRCHR
398 /**
399  * strrchr - Find the last occurrence of a character in a string
400  * @s: The string to be searched
401  * @c: The character to search for
402  */
403 char *strrchr(const char *s, int c)
404 {
405         const char *last = NULL;
406         do {
407                 if (*s == (char)c)
408                         last = s;
409         } while (*s++);
410         return (char *)last;
411 }
412 EXPORT_SYMBOL(strrchr);
413 #endif
414 
415 #ifndef __HAVE_ARCH_STRNCHR
416 /**
417  * strnchr - Find a character in a length limited string
418  * @s: The string to be searched
419  * @count: The number of characters to be searched
420  * @c: The character to search for
421  */
422 char *strnchr(const char *s, size_t count, int c)
423 {
424         for (; count-- && *s != '\0'; ++s)
425                 if (*s == (char)c)
426                         return (char *)s;
427         return NULL;
428 }
429 EXPORT_SYMBOL(strnchr);
430 #endif
431 
432 /**
433  * skip_spaces - Removes leading whitespace from @str.
434  * @str: The string to be stripped.
435  *
436  * Returns a pointer to the first non-whitespace character in @str.
437  */
438 char *skip_spaces(const char *str)
439 {
440         while (isspace(*str))
441                 ++str;
442         return (char *)str;
443 }
444 EXPORT_SYMBOL(skip_spaces);
445 
446 /**
447  * strim - Removes leading and trailing whitespace from @s.
448  * @s: The string to be stripped.
449  *
450  * Note that the first trailing whitespace is replaced with a %NUL-terminator
451  * in the given string @s. Returns a pointer to the first non-whitespace
452  * character in @s.
453  */
454 char *strim(char *s)
455 {
456         size_t size;
457         char *end;
458 
459         size = strlen(s);
460         if (!size)
461                 return s;
462 
463         end = s + size - 1;
464         while (end >= s && isspace(*end))
465                 end--;
466         *(end + 1) = '\0';
467 
468         return skip_spaces(s);
469 }
470 EXPORT_SYMBOL(strim);
471 
472 #ifndef __HAVE_ARCH_STRLEN
473 /**
474  * strlen - Find the length of a string
475  * @s: The string to be sized
476  */
477 size_t strlen(const char *s)
478 {
479         const char *sc;
480 
481         for (sc = s; *sc != '\0'; ++sc)
482                 /* nothing */;
483         return sc - s;
484 }
485 EXPORT_SYMBOL(strlen);
486 #endif
487 
488 #ifndef __HAVE_ARCH_STRNLEN
489 /**
490  * strnlen - Find the length of a length-limited string
491  * @s: The string to be sized
492  * @count: The maximum number of bytes to search
493  */
494 size_t strnlen(const char *s, size_t count)
495 {
496         const char *sc;
497 
498         for (sc = s; count-- && *sc != '\0'; ++sc)
499                 /* nothing */;
500         return sc - s;
501 }
502 EXPORT_SYMBOL(strnlen);
503 #endif
504 
505 #ifndef __HAVE_ARCH_STRSPN
506 /**
507  * strspn - Calculate the length of the initial substring of @s which only contain letters in @accept
508  * @s: The string to be searched
509  * @accept: The string to search for
510  */
511 size_t strspn(const char *s, const char *accept)
512 {
513         const char *p;
514         const char *a;
515         size_t count = 0;
516 
517         for (p = s; *p != '\0'; ++p) {
518                 for (a = accept; *a != '\0'; ++a) {
519                         if (*p == *a)
520                                 break;
521                 }
522                 if (*a == '\0')
523                         return count;
524                 ++count;
525         }
526         return count;
527 }
528 
529 EXPORT_SYMBOL(strspn);
530 #endif
531 
532 #ifndef __HAVE_ARCH_STRCSPN
533 /**
534  * strcspn - Calculate the length of the initial substring of @s which does not contain letters in @reject
535  * @s: The string to be searched
536  * @reject: The string to avoid
537  */
538 size_t strcspn(const char *s, const char *reject)
539 {
540         const char *p;
541         const char *r;
542         size_t count = 0;
543 
544         for (p = s; *p != '\0'; ++p) {
545                 for (r = reject; *r != '\0'; ++r) {
546                         if (*p == *r)
547                                 return count;
548                 }
549                 ++count;
550         }
551         return count;
552 }
553 EXPORT_SYMBOL(strcspn);
554 #endif
555 
556 #ifndef __HAVE_ARCH_STRPBRK
557 /**
558  * strpbrk - Find the first occurrence of a set of characters
559  * @cs: The string to be searched
560  * @ct: The characters to search for
561  */
562 char *strpbrk(const char *cs, const char *ct)
563 {
564         const char *sc1, *sc2;
565 
566         for (sc1 = cs; *sc1 != '\0'; ++sc1) {
567                 for (sc2 = ct; *sc2 != '\0'; ++sc2) {
568                         if (*sc1 == *sc2)
569                                 return (char *)sc1;
570                 }
571         }
572         return NULL;
573 }
574 EXPORT_SYMBOL(strpbrk);
575 #endif
576 
577 #ifndef __HAVE_ARCH_STRSEP
578 /**
579  * strsep - Split a string into tokens
580  * @s: The string to be searched
581  * @ct: The characters to search for
582  *
583  * strsep() updates @s to point after the token, ready for the next call.
584  *
585  * It returns empty tokens, too, behaving exactly like the libc function
586  * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
587  * Same semantics, slimmer shape. ;)
588  */
589 char *strsep(char **s, const char *ct)
590 {
591         char *sbegin = *s;
592         char *end;
593 
594         if (sbegin == NULL)
595                 return NULL;
596 
597         end = strpbrk(sbegin, ct);
598         if (end)
599                 *end++ = '\0';
600         *s = end;
601         return sbegin;
602 }
603 EXPORT_SYMBOL(strsep);
604 #endif
605 
606 /**
607  * sysfs_streq - return true if strings are equal, modulo trailing newline
608  * @s1: one string
609  * @s2: another string
610  *
611  * This routine returns true iff two strings are equal, treating both
612  * NUL and newline-then-NUL as equivalent string terminations.  It's
613  * geared for use with sysfs input strings, which generally terminate
614  * with newlines but are compared against values without newlines.
615  */
616 bool sysfs_streq(const char *s1, const char *s2)
617 {
618         while (*s1 && *s1 == *s2) {
619                 s1++;
620                 s2++;
621         }
622 
623         if (*s1 == *s2)
624                 return true;
625         if (!*s1 && *s2 == '\n' && !s2[1])
626                 return true;
627         if (*s1 == '\n' && !s1[1] && !*s2)
628                 return true;
629         return false;
630 }
631 EXPORT_SYMBOL(sysfs_streq);
632 
633 /**
634  * match_string - matches given string in an array
635  * @array:      array of strings
636  * @n:          number of strings in the array or -1 for NULL terminated arrays
637  * @string:     string to match with
638  *
639  * Return:
640  * index of a @string in the @array if matches, or %-EINVAL otherwise.
641  */
642 int match_string(const char * const *array, size_t n, const char *string)
643 {
644         int index;
645         const char *item;
646 
647         for (index = 0; index < n; index++) {
648                 item = array[index];
649                 if (!item)
650                         break;
651                 if (!strcmp(item, string))
652                         return index;
653         }
654 
655         return -EINVAL;
656 }
657 EXPORT_SYMBOL(match_string);
658 
659 #ifndef __HAVE_ARCH_MEMSET
660 /**
661  * memset - Fill a region of memory with the given value
662  * @s: Pointer to the start of the area.
663  * @c: The byte to fill the area with
664  * @count: The size of the area.
665  *
666  * Do not use memset() to access IO space, use memset_io() instead.
667  */
668 void *memset(void *s, int c, size_t count)
669 {
670         char *xs = s;
671 
672         while (count--)
673                 *xs++ = c;
674         return s;
675 }
676 EXPORT_SYMBOL(memset);
677 #endif
678 
679 /**
680  * memzero_explicit - Fill a region of memory (e.g. sensitive
681  *                    keying data) with 0s.
682  * @s: Pointer to the start of the area.
683  * @count: The size of the area.
684  *
685  * Note: usually using memset() is just fine (!), but in cases
686  * where clearing out _local_ data at the end of a scope is
687  * necessary, memzero_explicit() should be used instead in
688  * order to prevent the compiler from optimising away zeroing.
689  *
690  * memzero_explicit() doesn't need an arch-specific version as
691  * it just invokes the one of memset() implicitly.
692  */
693 void memzero_explicit(void *s, size_t count)
694 {
695         memset(s, 0, count);
696         barrier_data(s);
697 }
698 EXPORT_SYMBOL(memzero_explicit);
699 
700 #ifndef __HAVE_ARCH_MEMCPY
701 /**
702  * memcpy - Copy one area of memory to another
703  * @dest: Where to copy to
704  * @src: Where to copy from
705  * @count: The size of the area.
706  *
707  * You should not use this function to access IO space, use memcpy_toio()
708  * or memcpy_fromio() instead.
709  */
710 void *memcpy(void *dest, const void *src, size_t count)
711 {
712         char *tmp = dest;
713         const char *s = src;
714 
715         while (count--)
716                 *tmp++ = *s++;
717         return dest;
718 }
719 EXPORT_SYMBOL(memcpy);
720 #endif
721 
722 #ifndef __HAVE_ARCH_MEMMOVE
723 /**
724  * memmove - Copy one area of memory to another
725  * @dest: Where to copy to
726  * @src: Where to copy from
727  * @count: The size of the area.
728  *
729  * Unlike memcpy(), memmove() copes with overlapping areas.
730  */
731 void *memmove(void *dest, const void *src, size_t count)
732 {
733         char *tmp;
734         const char *s;
735 
736         if (dest <= src) {
737                 tmp = dest;
738                 s = src;
739                 while (count--)
740                         *tmp++ = *s++;
741         } else {
742                 tmp = dest;
743                 tmp += count;
744                 s = src;
745                 s += count;
746                 while (count--)
747                         *--tmp = *--s;
748         }
749         return dest;
750 }
751 EXPORT_SYMBOL(memmove);
752 #endif
753 
754 #ifndef __HAVE_ARCH_MEMCMP
755 /**
756  * memcmp - Compare two areas of memory
757  * @cs: One area of memory
758  * @ct: Another area of memory
759  * @count: The size of the area.
760  */
761 #undef memcmp
762 __visible int memcmp(const void *cs, const void *ct, size_t count)
763 {
764         const unsigned char *su1, *su2;
765         int res = 0;
766 
767         for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
768                 if ((res = *su1 - *su2) != 0)
769                         break;
770         return res;
771 }
772 EXPORT_SYMBOL(memcmp);
773 #endif
774 
775 #ifndef __HAVE_ARCH_MEMSCAN
776 /**
777  * memscan - Find a character in an area of memory.
778  * @addr: The memory area
779  * @c: The byte to search for
780  * @size: The size of the area.
781  *
782  * returns the address of the first occurrence of @c, or 1 byte past
783  * the area if @c is not found
784  */
785 void *memscan(void *addr, int c, size_t size)
786 {
787         unsigned char *p = addr;
788 
789         while (size) {
790                 if (*p == c)
791                         return (void *)p;
792                 p++;
793                 size--;
794         }
795         return (void *)p;
796 }
797 EXPORT_SYMBOL(memscan);
798 #endif
799 
800 #ifndef __HAVE_ARCH_STRSTR
801 /**
802  * strstr - Find the first substring in a %NUL terminated string
803  * @s1: The string to be searched
804  * @s2: The string to search for
805  */
806 char *strstr(const char *s1, const char *s2)
807 {
808         size_t l1, l2;
809 
810         l2 = strlen(s2);
811         if (!l2)
812                 return (char *)s1;
813         l1 = strlen(s1);
814         while (l1 >= l2) {
815                 l1--;
816                 if (!memcmp(s1, s2, l2))
817                         return (char *)s1;
818                 s1++;
819         }
820         return NULL;
821 }
822 EXPORT_SYMBOL(strstr);
823 #endif
824 
825 #ifndef __HAVE_ARCH_STRNSTR
826 /**
827  * strnstr - Find the first substring in a length-limited string
828  * @s1: The string to be searched
829  * @s2: The string to search for
830  * @len: the maximum number of characters to search
831  */
832 char *strnstr(const char *s1, const char *s2, size_t len)
833 {
834         size_t l2;
835 
836         l2 = strlen(s2);
837         if (!l2)
838                 return (char *)s1;
839         while (len >= l2) {
840                 len--;
841                 if (!memcmp(s1, s2, l2))
842                         return (char *)s1;
843                 s1++;
844         }
845         return NULL;
846 }
847 EXPORT_SYMBOL(strnstr);
848 #endif
849 
850 #ifndef __HAVE_ARCH_MEMCHR
851 /**
852  * memchr - Find a character in an area of memory.
853  * @s: The memory area
854  * @c: The byte to search for
855  * @n: The size of the area.
856  *
857  * returns the address of the first occurrence of @c, or %NULL
858  * if @c is not found
859  */
860 void *memchr(const void *s, int c, size_t n)
861 {
862         const unsigned char *p = s;
863         while (n-- != 0) {
864                 if ((unsigned char)c == *p++) {
865                         return (void *)(p - 1);
866                 }
867         }
868         return NULL;
869 }
870 EXPORT_SYMBOL(memchr);
871 #endif
872 
873 static void *check_bytes8(const u8 *start, u8 value, unsigned int bytes)
874 {
875         while (bytes) {
876                 if (*start != value)
877                         return (void *)start;
878                 start++;
879                 bytes--;
880         }
881         return NULL;
882 }
883 
884 /**
885  * memchr_inv - Find an unmatching character in an area of memory.
886  * @start: The memory area
887  * @c: Find a character other than c
888  * @bytes: The size of the area.
889  *
890  * returns the address of the first character other than @c, or %NULL
891  * if the whole buffer contains just @c.
892  */
893 void *memchr_inv(const void *start, int c, size_t bytes)
894 {
895         u8 value = c;
896         u64 value64;
897         unsigned int words, prefix;
898 
899         if (bytes <= 16)
900                 return check_bytes8(start, value, bytes);
901 
902         value64 = value;
903 #if defined(CONFIG_ARCH_HAS_FAST_MULTIPLIER) && BITS_PER_LONG == 64
904         value64 *= 0x0101010101010101ULL;
905 #elif defined(CONFIG_ARCH_HAS_FAST_MULTIPLIER)
906         value64 *= 0x01010101;
907         value64 |= value64 << 32;
908 #else
909         value64 |= value64 << 8;
910         value64 |= value64 << 16;
911         value64 |= value64 << 32;
912 #endif
913 
914         prefix = (unsigned long)start % 8;
915         if (prefix) {
916                 u8 *r;
917 
918                 prefix = 8 - prefix;
919                 r = check_bytes8(start, value, prefix);
920                 if (r)
921                         return r;
922                 start += prefix;
923                 bytes -= prefix;
924         }
925 
926         words = bytes / 8;
927 
928         while (words) {
929                 if (*(u64 *)start != value64)
930                         return check_bytes8(start, value, 8);
931                 start += 8;
932                 words--;
933         }
934 
935         return check_bytes8(start, value, bytes % 8);
936 }
937 EXPORT_SYMBOL(memchr_inv);
938 
939 /**
940  * strreplace - Replace all occurrences of character in string.
941  * @s: The string to operate on.
942  * @old: The character being replaced.
943  * @new: The character @old is replaced with.
944  *
945  * Returns pointer to the nul byte at the end of @s.
946  */
947 char *strreplace(char *s, char old, char new)
948 {
949         for (; *s; ++s)
950                 if (*s == old)
951                         *s = new;
952         return s;
953 }
954 EXPORT_SYMBOL(strreplace);
