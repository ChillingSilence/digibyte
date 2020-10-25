digibyte/randomx / notes
barrystyle 27092020 (https://github.com/barrystyle)



in wanting to keep randomx/digibyte compatible with xmrig;
moneros' block header is 76b total, with the nonce positioned at offset 39 (right in the middle) rather
than the last 4 bytes of serialization (like bitcoin). to squeeze the header in, nbits is dropped from the hash input.
nbits still must be correct when the block is submitted to the daemon; and it still gets written to disk (the serialize_monero_hash
function is only used for calculating the pow hash).

at epoch time, the merkleroot of the previous epoch's block is used to populate the seedhash.

of particular annoyance is the randomx_init_cache function, a no-op if the seed has not changed, documented as reinitializing the
cache if it has. this is not the case and the vm/cache must be destroyed/reinitialized. as well.. for some fun, try running randomx under valgrind.. !



typical monero blob (76byte):
0c0ca395bafb05ed4b92540beac3405871b8dcea19d4717e8a5de458a7a11fc24b9ef1a6e4a76300000000d874d73c1df07341dd4a412a191fc2b8b69d4f745930cbf12195e4ae8af463d30c

typical monero seed:
3f10b642432cd8e4e5702b28c0f396429d856640bd44c7127e5c80450f27f39f

typical satoshi blockheader:
AAAAAAAA nversion (+4)
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB prevhash (+32)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC mrklroot (+32)
DDDDDDDD ntime (+4)
EEEEEEEE nbits (+4)
FFFFFFFF nonce (+4)

adaption:
0c0ca395bafb05ed4b92540beac3405871b8dcea19d4717e8a5de458a7a11fc24b9ef1a6e4a76300000000d874d73c1df07341dd4a412a191fc2b8b69d4f745930cbf12195e4ae8af463d30c
AAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBCCCCCCFFFFFFFFCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCDDDDDDDD

the original blockheader gets stored to disk untouched, this is only for calculating the randomx pow hash

