.PHONY: all test
.ONESHELL:

all:
	make -s test

test:
	cd hashtable
	make test
	./test >my.out
	diff -s --strip-trailing-cr ../ref_hash.out my.out
	cd ../btree/rec
	make test
	./test >my.out
	diff -s --strip-trailing-cr ../../ref_rec.out my.out
# 	cd ../iter
# 	make test
# 	./test >my.out
# 	diff -s --strip-trailing-cr ../../ref_iter.out my.out
# 	cd ../exa
# 	make test
# 	./test_iter >my_iter.out
# 	diff -s --strip-trailing-cr ../../ref_exa_iter.out my_iter.out
# 	./test_rec >my_rec.out
# 	diff -s --strip-trailing-cr ../../ref_exa_rec.out my_rec.out