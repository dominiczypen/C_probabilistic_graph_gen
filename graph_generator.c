/* Author: Dominic van der Zypen
* Last modified: 2022-08-24
* Goal: Generate adjacency list for probabilistic graphs
* and also adjacency matrix
* The edge probability is determined by the variable edge_probability_level,
* which is is an integer in the closed interval
* [0,0xf] = [0,15]. That number is divided by 16 to give the edge
* edge probability of the graph being generated. So if edge_prob
* is 8, then edge probability is 50%, and if edge_prob = 4, then
* edge probability is 25%.
*/

#include <stdio.h>
#include <stdlib.h> // used for writing files
#include <stdint.h> // used for uint32_t

//---------------------

// Function declarations

void lfsr(uint32_t* x32); // linear feedback shift register for
                          // high quality random numbers
void set_parameters(unsigned int* num_ver_p, unsigned int* edge_prob_p);
void generate_adjacency_list(unsigned int num_vertices, unsigned int edge_probability_level);
void print_uint32_bin(uint32_t x); // only for testing

//---------------------

void lfsr(uint32_t* x32)
  // taps 32, 16, 7, 2 ->
  // irreducible binary polynomial x^{32} + x^{16} + x^{7} + x^2 + 1

{
  uint32_t my_bit; // bit that is the XOR of the taps at positions
                   // 32, 16, 7, 2
  uint32_t tmp = *x32;
  my_bit = (tmp ^ (tmp >> 16) ^ (tmp >> 25) ^ (tmp >> 30)) & 1u;
  tmp = (tmp >> 1) | (my_bit << 31);
  *x32 = tmp;
}

//---------------------

void set_parameters(unsigned int* num_ver_p, unsigned int* edge_prob_p)
  // trivial function to set number of vertices and edge
  // probability in percent (%).
{
  printf("Enter numbers of vertices: ");
  scanf("%d", num_ver_p); // num_p is pointer
  unsigned char scanf_loop = 1;
      // used for loop: number to be entered has to be between 1 and 15
  while (scanf_loop)
  {
    printf("Enter edge probability level x between 1 and 15 ");
    printf("(The edge probability will be x/16 . Binary number  16 is used\n");
    printf("for speed purposes (bit operations)) : ");
    scanf("%d", edge_prob_p);
    if ((*edge_prob_p) && (((*edge_prob_p) & 0xf) == *edge_prob_p))
    // meaning: if *edge_prob_p > 0 and < 16
    {
      scanf_loop = 0;
    }
  }
}

//---------------------

void generate_adjacency_list(unsigned int num_vertices, unsigned int edge_probability_level)
  // write file "adjacency_list.txt" in same directory as this program is run
  // Format of each line : x, y, b
  // where x < y are vertex numbers, and the binary value b indicates
  // whether x and y are connected by an edge (b == 1), or not (b == 0).
  // Vertices are numbered from 0 to (num_vertices - 1).
{
  unsigned int i = 0;
  unsigned int j = 1;
  unsigned int edge_bit; // 0 or 1 depending on whether there shall be an edge

  uint32_t my32 = 0xb16b00b5; // some standard 32-bit number :)

  FILE* fptr;
  fptr = fopen("adjacency_list.txt", "w");

  while (i < num_vertices)
  {
    while (j < num_vertices)
    {
      edge_bit = 0; // default : no edge!

      if ((my32 & 0xf) < edge_probability_level)
           // ... & 0xf is modulo 16!!
      {
        edge_bit = 1;
      }

      // - - - - print 1 line of i, y, edge_bit to file
      fprintf(fptr, "%d", i);
      fprintf(fptr, "%s", ", ");
      fprintf(fptr, "%d", j);
      fprintf(fptr, "%s", ", ");
      fprintf(fptr, "%d", edge_bit);
      // -- DEBUG
      // fprintf(fptr, "%s", ", ");
      // fprintf(fptr, "%x", my32);
      // -- DEBUG END
      fprintf(fptr, "%s", "\n");
      // - - - -

      lfsr(&my32); // generate next value for my32 by LFSR operation
      j++;
    }
    i++;
    j = i + 1; // set j back after j reached maximum
  }
  fclose(fptr);
}

//---------------------

void print_uint32_bin(uint32_t x)
{
  char i = 0;
  char bit;
  uint32_t u1000 = (((uint32_t) 1u) << 31);
  while (i < 32)
  {
    bit = (x & u1000) >> 31;
    printf("%d", bit);
    x = x << 1;
    i++;
    if (!(i & 3)) {printf(" ");} // formatting
  }
}

//---------------------

int main()
{
  unsigned int num_vertices, edge_probability_level;
     // edge_probability_level is an integer in the closed interval
     // [0,0xf] = [0,15]. That number is divided by 16 to give the edge
     // edge probability of the graph being generated. So if edge_prob
     // is 8, then edge probability is 50%, and if edge_prob = 4, then
     // edge probability is 25%.

  set_parameters(&num_vertices, &edge_probability_level);
  generate_adjacency_list(num_vertices, edge_probability_level);

  return 0;
}
