//**************************************************************************
// Multi-threaded Matrix Multiply benchmark
//--------------------------------------------------------------------------
// TA     : Christopher Celio
// Student: 
//
//
// This benchmark multiplies two 2-D arrays together and writes the results to
// a third vector. The input data (and reference data) should be generated
// using the matmul_gendata.pl perl script and dumped to a file named
// dataset.h. 


// print out arrays, etc.
//#define DEBUG

//--------------------------------------------------------------------------
// Includes 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//--------------------------------------------------------------------------
// Input/Reference Data

typedef float data_t;
#include "dataset.h"
 
  
//--------------------------------------------------------------------------
// Basic Utilities and Multi-thread Support

__thread unsigned long coreid;
unsigned long ncores;

#include "util.h"
   
#define stringify_1(s) #s
#define stringify(s) stringify_1(s)
#define stats(code) do { \
    unsigned long _c = -rdcycle(), _i = -rdinstret(); \
    code; \
    _c += rdcycle(), _i += rdinstret(); \
    if (coreid == 0) \
      printf("%s: %ld cycles, %ld.%ld cycles/iter, %ld.%ld CPI\n", \
             stringify(code), _c, _c/DIM_SIZE/DIM_SIZE/DIM_SIZE, 10*_c/DIM_SIZE/DIM_SIZE/DIM_SIZE%10, _c/_i, 10*_c/_i%10); \
  } while(0)
 

//--------------------------------------------------------------------------
// Helper functions
    
void printArray( char name[], int n, data_t arr[] )
{
   int i;
   if (coreid != 0)
      return;
  
   printf( " %10s :", name );
   for ( i = 0; i < n; i++ )
      printf( " %3ld ", (long) arr[i] );
   printf( "\n" );
}
      
void __attribute__((noinline)) verify(size_t n, const data_t* test, const data_t* correct)
{
   if (coreid != 0)
      return;

   size_t i;
   for (i = 0; i < n; i++)
   {
      if (test[i] != correct[i])
      {
         printf("FAILED test[%d]= %3ld, correct[%d]= %3ld\n", 
            i, (long)test[i], i, (long)correct[i]);
         exit(-1);
      }
   }
   
   return;
}
 
//--------------------------------------------------------------------------
// matmul function
 
// single-thread, naive version
void __attribute__((noinline)) matmul_naive(const int lda,  const data_t A[], const data_t B[], data_t C[] )
{
///*
   int i, j, k;

   if (coreid > 0)
      return;
  
   for ( i = 0; i < lda; i++ )
      for ( j = 0; j < lda; j++ )  
      {
         for ( k = 0; k < lda; k++ ) 
         {
            C[i + j*lda] += A[j*lda + k] * B[k*lda + i];
         }
      }
//*/
  /*
   int i, j, k, kk; 
  if (coreid) { 
   for ( i = 0; i < 16; i+=8 )
   {
      for ( j = 0; j < 32; j++ )  
      {
         data_t temp0 = 0;
         data_t temp1 = 0;
         data_t temp2 = 0;
         data_t temp3 = 0;
         data_t temp4 = 0;
         data_t temp5 = 0;
         data_t temp6 = 0;
         data_t temp7 = 0;
         for ( kk = 0; kk < 32; kk+=8 ) 
         for ( k = kk; k < kk+8; k++ ) 
//       for ( k = 0; k < 32; k++ ) 
         {
            data_t tempA = A[j*32+k];
            temp0 += tempA * B[k*32 + i];
            temp1 += tempA * B[k*32 + i+1];
            temp2 += tempA * B[k*32 + i+2];
            temp3 += tempA * B[k*32 + i+3];
            temp4 += tempA * B[k*32 + i+4];
            temp5 += tempA * B[k*32 + i+5];
            temp6 += tempA * B[k*32 + i+6];
            temp7 += tempA * B[k*32 + i+7];
         }
         C[i+j*32] = temp0;
         C[i+j*32+1] = temp1;
         C[i+j*32+2] = temp2;
         C[i+j*32+3] = temp3;
         C[i+j*32+4] = temp4;
         C[i+j*32+5] = temp5;
         C[i+j*32+6] = temp6;
         C[i+j*32+7] = temp7;
      }
   } 
  } else {
   for ( i =  16; i < 32; i+=8 )
   {
      for ( j = 0; j < 32; j++ )  
      {
         data_t temp0 = 0;
         data_t temp1 = 0;
         data_t temp2 = 0;
         data_t temp3 = 0;
         data_t temp4 = 0;
         data_t temp5 = 0;
         data_t temp6 = 0;
         data_t temp7 = 0;
         for ( kk = 0; kk < 32; kk+=8 ) 
         for ( k = kk; k < kk+8; k++ ) 
         {
            data_t tempA = A[j*32+k];
            temp0 += tempA * B[k*32 + i];
            temp1 += tempA * B[k*32 + i+1];
            temp2 += tempA * B[k*32 + i+2];
            temp3 += tempA * B[k*32 + i+3];
            temp4 += tempA * B[k*32 + i+4];
            temp5 += tempA * B[k*32 + i+5];
            temp6 += tempA * B[k*32 + i+6];
            temp7 += tempA * B[k*32 + i+7];
         }
         C[i+j*32] = temp0;
         C[i+j*32+1] = temp1;
         C[i+j*32+2] = temp2;
         C[i+j*32+3] = temp3;
         C[i+j*32+4] = temp4;
         C[i+j*32+5] = temp5;
         C[i+j*32+6] = temp6;
         C[i+j*32+7] = temp7;
      }

   }
  }
  */ 
}
 

void __attribute__((noinline)) matmul(const int lda,  const data_t A[], const data_t B[], data_t C[] )
{
   
   // ***************************** //
   // **** ADD YOUR CODE HERE ***** //
   // ***************************** //
   //
   // feel free to make a separate function for MI and MSI versions.
   int i, j, k, ii, jj, kk; 
  if (coreid) { 
// for ( ii = 0; ii < 32; ii+=IC )
         for ( kk = 0; kk < 32; kk+=16 ) 
   for ( j = 0; j < 16; j++ )  
// for ( j = 0; j < 16; j++ )  
   {
      for ( i =  0; i < 32; i+=8 )
//    for ( i = ii; i < ii + IC && i < 32; i+=8 )
      {
         data_t temp0 = C[i+j*32];
         data_t temp1 = C[i+j*32+1];
         data_t temp2 = C[i+j*32+2];
         data_t temp3 = C[i+j*32+3];
         data_t temp4 = C[i+j*32+4];
         data_t temp5 = C[i+j*32+5];
         data_t temp6 = C[i+j*32+6];
         data_t temp7 = C[i+j*32+7];
         for ( k = kk; k < kk+16 && k < 32; k++ ) 
//       for ( k = 0; k < 32; k++ ) 
         {
            data_t tempA = A[j*32+k];
            temp0 += tempA * B[k*32 + i];
            temp1 += tempA * B[k*32 + i+1];
            temp2 += tempA * B[k*32 + i+2];
            temp3 += tempA * B[k*32 + i+3];
            temp4 += tempA * B[k*32 + i+4];
            temp5 += tempA * B[k*32 + i+5];
            temp6 += tempA * B[k*32 + i+6];
            temp7 += tempA * B[k*32 + i+7];
         }
         C[i+j*32] = temp0;
         C[i+j*32+1] = temp1;
         C[i+j*32+2] = temp2;
         C[i+j*32+3] = temp3;
         C[i+j*32+4] = temp4;
         C[i+j*32+5] = temp5;
         C[i+j*32+6] = temp6;
         C[i+j*32+7] = temp7;
      }
   } 
  } else {
// for ( ii = 0; ii < 32; ii+=IC )
         for ( kk = 0; kk < 32; kk+=16 ) 
   for ( j = 16; j < 32; j++ )  
// for ( j = 16; j < 32; j++ )  
   {
      for ( i =   0; i < 32; i+=8 )
//    for ( i = ii; i < ii + IC && i < 32; i+=8 )
      {
         data_t temp0 = C[i+j*32];
         data_t temp1 = C[i+j*32+1];
         data_t temp2 = C[i+j*32+2];
         data_t temp3 = C[i+j*32+3];
         data_t temp4 = C[i+j*32+4];
         data_t temp5 = C[i+j*32+5];
         data_t temp6 = C[i+j*32+6];
         data_t temp7 = C[i+j*32+7];
         for ( k = kk; k < kk+16 && k < 32; k++ ) 
         {
            data_t tempA = A[j*32+k];
            temp0 += tempA * B[k*32 + i];
            temp1 += tempA * B[k*32 + i+1];
            temp2 += tempA * B[k*32 + i+2];
            temp3 += tempA * B[k*32 + i+3];
            temp4 += tempA * B[k*32 + i+4];
            temp5 += tempA * B[k*32 + i+5];
            temp6 += tempA * B[k*32 + i+6];
            temp7 += tempA * B[k*32 + i+7];
         }
         C[i+j*32] = temp0;
         C[i+j*32+1] = temp1;
         C[i+j*32+2] = temp2;
         C[i+j*32+3] = temp3;
         C[i+j*32+4] = temp4;
         C[i+j*32+5] = temp5;
         C[i+j*32+6] = temp6;
         C[i+j*32+7] = temp7;
      }

   }
  } 
}

//--------------------------------------------------------------------------
// Main
//
// all threads start executing thread_entry(). Use their "coreid" to
// differentiate between threads (each thread is running on a separate core).
  
void thread_entry(int cid, int nc)
{
   coreid = cid;
   ncores = nc;

   // static allocates data in the binary, which is visible to both threads
   static data_t results_data[ARRAY_SIZE];


   // Execute the provided, naive matmul
   barrier();
   stats(matmul_naive(DIM_SIZE, input1_data, input2_data, results_data); barrier());
 
   
   // verify
   verify(ARRAY_SIZE, results_data, verify_data);
   
   // clear results from the first trial
   size_t i;
   if (coreid == 0) 
      for (i=0; i < ARRAY_SIZE; i++)
         results_data[i] = 0;
   barrier();

   
   // Execute your faster matmul
   barrier();
   stats(matmul(DIM_SIZE, input1_data, input2_data, results_data); barrier());
 
#ifdef DEBUG
   printArray("results:", ARRAY_SIZE, results_data);
   printArray("verify :", ARRAY_SIZE, verify_data);
#endif
   
   // verify
   verify(ARRAY_SIZE, results_data, verify_data);
   barrier();

   exit(0);
}

