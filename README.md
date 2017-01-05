# PBMUFHE
The source code is the paper "POLICY-BASED, MULTI-USER, FULLY ENCRYPTION SCHEME FOR CLOUD COMPUTING" proposed in the program algorithm. 

My study proposes a policy-based, multi-user, fully homomorphic encryption algorithm (PB-MUFHE) that accounts for user diversity in the 
cloud environment, in which, the encrypted data is set under an appropriate access policy, the key to which is a set attribute. Not only
is homomorphic evaluation supported for ciphertexts among several users, but also fine-grained access control and sharing between users 
are supported. The security of the PB-MUFHE scheme was tested based on learning with errors (LWE) problems, and IND-CPA security analysis
in the generic bilinear group random oracle model further verified the proposed algorithm’s effectiveness. Performance assessment 
demonstrated that, PB-MUFHE can efficiently implement fully homomorphic evaluation of ciphertext, and effectively support access control
and shared multi-user capability.

After introduce my paper, Let's talk about how to compile the source code. Before executing the make command, you must ensure that the 
following static libraries are installed：
pthread,crypto,gmp,pbc,ntl. 
if they are installed above libraries, will installed in /usr/local/lib.
and glib-2.0,gthread-2.0 

If you have installed the static libraries above, first you should executing the make command in directory ABE_FHE, it will generate the static
library abefhe.a; then, you can executing the make command in directory PBMUFHE, that will generate 5 executable file, setup, prvGen, abefheDec,
abefheEnc, abefheCom.
