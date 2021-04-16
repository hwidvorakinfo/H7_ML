/* Generated by Edge Impulse
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
// Generated on: 16.04.2021 08:23:10

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "edge-impulse-sdk/tensorflow/lite/c/builtin_op_data.h"
#include "edge-impulse-sdk/tensorflow/lite/c/common.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/kernels/micro_ops.h"

#if EI_CLASSIFIER_PRINT_STATE
#if defined(__cplusplus) && EI_C_LINKAGE == 1
extern "C" {
    extern void ei_printf(const char *format, ...);
}
#else
extern void ei_printf(const char *format, ...);
#endif
#endif

#if defined __GNUC__
#define ALIGN(X) __attribute__((aligned(X)))
#elif defined _MSC_VER
#define ALIGN(X) __declspec(align(X))
#elif defined __TASKING__
#define ALIGN(X) __align(X)
#endif

namespace {

constexpr int kTensorArenaSize = 208;

#if defined(EI_CLASSIFIER_ALLOCATION_STATIC)
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#elif defined(EI_CLASSIFIER_ALLOCATION_STATIC_HIMAX)
#pragma Bss(".tensor_arena")
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#pragma Bss()
#elif defined(EI_CLASSIFIER_ALLOCATION_STATIC_HIMAX_GNU)
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16) __attribute__((section(".tensor_arena")));
#else
#define EI_CLASSIFIER_ALLOCATION_HEAP 1
uint8_t* tensor_arena = NULL;
#endif

static uint8_t* tensor_boundary;
static uint8_t* current_location;

template <int SZ, class T> struct TfArray {
  int sz; T elem[SZ];
};
enum used_operators_e {
  OP_FULLY_CONNECTED, OP_SOFTMAX,  OP_LAST
};
struct TensorInfo_t { // subset of TfLiteTensor used for initialization from constant memory
  TfLiteAllocationType allocation_type;
  TfLiteType type;
  void* data;
  TfLiteIntArray* dims;
  size_t bytes;
  TfLiteQuantization quantization;
};
struct NodeInfo_t { // subset of TfLiteNode used for initialization from constant memory
  struct TfLiteIntArray* inputs;
  struct TfLiteIntArray* outputs;
  void* builtin_data;
  used_operators_e used_op_index;
};

TfLiteContext ctx{};
TfLiteTensor tflTensors[11];
TfLiteRegistration registrations[OP_LAST];
TfLiteNode tflNodes[4];

const TfArray<2, int> tensor_dimension0 = { 2, { 1,100 } };
const TfArray<1, float> quant0_scale = { 1, { 0.19600977003574371, } };
const TfArray<1, int> quant0_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant0 = { (TfLiteFloatArray*)&quant0_scale, (TfLiteIntArray*)&quant0_zero, 0 };
const ALIGN(8) int32_t tensor_data1[20] = { -24, -12, 16, -2, 17, -8, 17, -5, 38, 27, 2, 25, -12, -22, 11, -23, 26, 25, -3, -15, };
const TfArray<1, int> tensor_dimension1 = { 1, { 20 } };
const TfArray<1, float> quant1_scale = { 1, { 0.00035893684253096581, } };
const TfArray<1, int> quant1_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant1 = { (TfLiteFloatArray*)&quant1_scale, (TfLiteIntArray*)&quant1_zero, 0 };
const ALIGN(8) int32_t tensor_data2[10] = { -5, 0, 43, -6, -8, -9, -11, 12, -12, 12, };
const TfArray<1, int> tensor_dimension2 = { 1, { 10 } };
const TfArray<1, float> quant2_scale = { 1, { 0.00086021784227341413, } };
const TfArray<1, int> quant2_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant2 = { (TfLiteFloatArray*)&quant2_scale, (TfLiteIntArray*)&quant2_zero, 0 };
const ALIGN(8) int32_t tensor_data3[2] = { 15, -15, };
const TfArray<1, int> tensor_dimension3 = { 1, { 2 } };
const TfArray<1, float> quant3_scale = { 1, { 0.0011666165664792061, } };
const TfArray<1, int> quant3_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant3 = { (TfLiteFloatArray*)&quant3_scale, (TfLiteIntArray*)&quant3_zero, 0 };
const ALIGN(8) int8_t tensor_data4[20*100] = { 
  -94, -89, -77, 10, -25, 6, -107, -93, -101, 13, 104, -65, -87, 101, 45, 113, 105, -43, 78, -82, 99, -114, 108, -93, -75, -75, -115, 44, -52, -47, -81, -112, 18, 101, 31, -55, -67, 76, 29, -17, 77, 113, 74, 58, -104, 31, -32, 52, -59, 15, 55, 90, -106, 90, 12, 21, 9, -28, -21, -1, 9, -37, -67, -37, -61, -37, 53, 50, -7, -95, 98, -76, -16, 5, -27, 107, 67, 84, -14, 66, 74, 49, 11, 74, -14, 28, -50, 34, -24, 79, 92, -97, 12, 56, -3, -44, 20, 41, -1, 111, 
  19, 30, 4, -44, 27, 116, 79, 101, -95, 82, 66, -55, -23, -100, -66, -46, -43, -98, 86, 57, -6, -63, -93, -65, 41, -66, 33, 60, -62, 9, 99, 113, -114, -18, -107, 108, -37, 69, 86, -56, -59, 24, 27, 7, -97, -109, 34, -3, -19, -63, 77, 122, -107, -55, 112, -17, -105, 121, -22, -87, 105, 12, -64, 76, -38, -58, 45, -78, 74, -26, -119, 64, -29, -64, 66, 16, -90, -104, 71, -107, 95, -68, -100, -117, -51, -14, -95, 97, -41, -109, 103, -4, -100, -82, -119, 104, 7, 105, -117, -113, 
  -4, -122, -99, 47, -4, 24, -119, -32, -79, 9, -44, -33, -93, -30, 60, -105, -86, -32, 97, -120, 78, -20, -64, 20, -58, 97, 9, -66, -23, -76, -82, 87, -109, 44, -85, 35, 25, -21, -63, -80, 95, -26, -8, -14, -32, 9, 81, -34, 117, -98, 108, -7, 18, -87, 73, 58, 70, 28, 33, 86, 39, -119, -110, 38, 52, -93, -61, 6, 60, 105, 84, 49, -48, 112, -45, 67, -87, -34, -93, -33, 25, -60, -91, 35, 95, 114, -14, 60, -89, 49, -42, 27, -119, -42, 115, 37, 77, -17, 54, -25, 
  -116, -30, -93, 53, 35, 38, -81, 22, 22, 107, 4, -102, 22, 57, 13, -56, -29, -22, 117, 74, -117, -73, 124, -7, -14, 114, 11, -37, 95, 25, 2, 91, 10, -97, -25, 92, 31, -22, -88, 7, 9, -20, -27, 68, -89, -74, 91, -89, 70, 75, 103, -68, -46, 111, -109, -68, 66, 4, 80, -81, -110, -63, -23, 79, -64, 38, -34, -116, -113, 47, 60, -40, 2, -70, 18, -84, -99, 51, -56, 73, 74, -64, -63, 18, 66, 69, 96, -59, 105, -34, -2, -77, -45, 78, 74, -98, -90, -48, 70, -8, 
  5, 72, -69, 80, 42, 112, -88, -43, 27, 41, 44, 8, -4, -21, -99, -1, -55, -50, 31, 26, 23, -45, -43, 31, -9, -51, 109, 53, 114, 3, 12, -60, -56, 71, -57, -42, 73, 66, -85, -58, 55, -95, -75, -28, -108, -2, 104, 95, 68, 43, 55, -105, 27, -62, 82, -22, 76, -89, 55, 38, -82, 29, -36, -66, 120, -4, 25, -74, -21, -113, -44, -5, -11, 29, 30, 49, 43, 34, -6, 116, -24, 64, 73, -69, 79, 0, -15, -51, -84, 107, -78, 53, 47, -43, 116, 4, -70, 52, 123, 17, 
  59, -70, -25, -22, -5, 43, 27, 38, -4, -103, -61, -76, 109, -59, 87, 20, -64, 41, 79, 100, -61, -115, -26, -41, -84, 53, 18, -50, 64, 38, 53, 112, 4, 107, -37, 73, -14, 31, 116, -67, -6, 29, 101, 63, -37, 74, -114, 71, -21, -13, -22, 3, -41, -55, -21, 73, 30, -49, 74, 10, -27, -3, 97, 4, 24, -4, 73, 46, 19, -90, -119, -53, -63, -114, -62, -113, 27, 118, 41, 51, -110, 26, 31, 1, 117, 111, -40, 31, -89, -36, 35, 31, -109, 23, -89, 3, 50, 109, -87, 47, 
  -87, 113, -12, -40, -43, 39, 39, -24, -53, 69, -96, -88, -116, -82, 108, 33, -21, 61, 36, -39, 26, -11, 115, -88, 34, 6, -56, 24, 34, -29, 57, -32, -72, -6, -40, -48, 31, 87, 15, -44, 99, 8, -101, -61, -14, -1, 92, 107, 68, 57, 47, 18, 85, -39, -110, 62, -64, 58, 12, 22, 51, -30, 22, -35, -46, -99, 43, -30, -51, -12, -105, 67, -18, 56, -43, -63, 32, 38, 99, 30, 61, 21, -37, 6, 63, -60, -47, 97, 111, 13, -42, -13, 7, 98, -104, 23, 80, -81, 116, 105, 
  -43, 115, -84, -30, -102, 109, 15, -115, 94, -96, 74, -68, -89, -55, -69, -15, -5, -98, 5, -36, -40, -98, -118, -26, -22, -107, 82, 35, 42, 82, -109, 99, -109, -65, 10, 35, -121, 24, 49, 39, 12, -111, 74, -82, -82, 89, 51, 92, 23, 1, 107, 36, -58, 108, 45, 11, -2, 104, -41, -72, -21, 58, 33, 88, -110, 36, 47, -85, 74, -74, -116, 99, -42, 114, 40, 55, 15, 5, 95, 91, 83, 102, -15, -12, -73, 36, 80, -100, -95, 108, -111, -103, -120, -112, -90, -76, -123, -93, -81, 64, 
  41, 53, 49, -111, 79, 60, -34, -20, 55, -91, -115, 119, 13, -43, -117, -70, 26, -16, 91, 3, -10, -30, -18, -60, -47, 111, -99, 32, 100, 2, -38, 49, -48, 52, 88, 41, -15, 18, -99, 34, 65, -95, -81, 84, -30, 11, 98, 63, -80, 106, -117, -86, 30, -80, -31, 4, -98, 46, 75, 39, -16, 93, -60, 123, 107, 110, -116, 21, 9, -117, 51, -76, 58, 123, -80, 115, 56, 69, 105, -14, -51, -26, 87, -37, 65, -7, 57, 48, 52, -19, 100, -16, -110, -26, 12, 52, -21, -105, 115, -85, 
  0, 29, 16, 65, -100, 0, -4, -41, 66, -16, 9, -34, -53, 0, 14, -46, 37, 112, 103, -93, 81, -71, -4, 106, 97, -84, -36, -71, 110, 74, -89, 42, 1, -84, -13, 108, -8, 94, 0, 93, -80, -46, 68, 75, 40, -88, 86, -7, -33, 91, -25, 125, -66, 123, 91, -109, -88, -42, 71, 40, -63, -37, 119, -73, 114, 106, 117, -62, 92, 57, 18, -46, -58, 28, 55, 6, 83, 85, 56, -72, 4, -90, -42, 40, -113, 64, -26, -38, 40, 16, -96, 37, 74, -82, -59, -16, 16, -79, -53, -76, 
  -97, -100, -115, -91, 113, -41, 39, -4, 16, 81, 9, 102, 23, 101, -14, -89, -76, 73, -38, -13, -66, 6, 76, -109, -53, 38, -88, 67, 42, -100, 115, -21, -112, -3, 105, 44, -44, 14, -78, -12, -49, 6, 97, 104, 39, 77, 23, 73, -50, -113, 109, -58, -97, 2, 73, -77, 16, 99, -116, 111, -68, 23, 70, -106, -88, -60, 98, -32, -118, -62, -50, -19, -68, -62, 37, -57, -97, -104, 63, 77, 85, 108, -65, -99, -85, 65, 64, -68, 53, 79, 80, -23, 37, -7, 24, -68, 99, -11, 97, 96, 
  102, 67, -4, -20, 14, 120, 32, 86, 95, 115, -24, 101, 17, -80, -71, 55, 19, 72, 95, 90, 62, -45, -3, 30, 86, -82, -24, 98, -74, 75, -104, 49, -78, -43, 88, -93, 98, 60, 98, -50, -49, -35, -119, -108, -102, -13, -119, 0, -5, 19, -29, -12, 99, 121, 117, -60, 96, 48, -112, 54, -29, -16, 77, 15, 12, 77, 89, -69, 45, 59, -81, -57, 5, 119, -39, 33, -76, 47, -65, -22, -91, -39, 52, 103, 31, -35, 88, 119, -50, -93, -4, 93, 38, -78, -104, -94, -6, 65, -25, -92, 
  49, 86, 25, -65, 97, -35, 36, -49, -13, 116, -5, -55, -83, 104, 110, -112, 65, -3, -111, -22, -108, -20, 112, -16, -97, -10, -85, 40, 98, -47, 84, -36, -35, 105, -100, -63, -60, -10, -86, 115, -24, 30, 25, 96, -7, -48, 75, -47, -15, 54, -33, -39, -35, -89, 100, 85, 88, 6, 74, 53, -87, -66, 45, -12, -26, 79, 111, 106, 107, -94, -111, -93, 36, -8, -96, 87, 6, 40, -59, 52, 14, -7, 109, -12, -68, 79, -73, -22, -14, -32, 99, 63, -88, 30, 14, 106, -115, 71, -95, -92, 
  9, 25, -40, 76, 11, -34, 55, -28, 32, 86, 112, 101, 54, -54, 1, 21, -75, -67, 28, 93, -122, -49, 46, -52, -124, 92, 62, 17, 100, -58, -124, 117, 30, 53, 79, 54, 103, -76, 64, -116, -43, -13, 75, -39, -74, 5, -71, -13, -7, 108, -82, -79, -18, -31, 73, 66, 18, 108, -114, -56, -8, 0, 112, -22, -5, 73, 37, 73, -126, 100, 90, -101, -4, 39, 105, -94, 41, 24, -44, 117, 26, -68, 61, -23, 68, 59, 72, -72, -88, 113, -31, 69, -83, -27, 52, -16, 68, 69, 81, 74, 
  -75, -124, 116, -61, 53, -45, 58, 71, 8, -48, -106, -24, -103, -36, 8, -23, -69, -59, -55, -74, 22, 46, -73, 40, 65, 1, -93, -21, -54, 4, 15, -7, 83, 76, 93, -120, 26, 7, -92, -45, -4, -99, 80, -111, -49, -5, 72, -121, 81, 8, -95, 3, 67, 55, -24, -59, -48, 9, 84, 90, -122, 71, -31, -36, -118, 5, 66, 101, -92, -113, 82, -22, 91, 0, -40, 30, 92, -16, -36, -10, 53, -116, -55, 53, -50, 68, -30, 59, -98, -72, 31, 117, 75, -97, 65, -13, -120, -34, -4, -67, 
  -73, 117, -36, 42, -28, 106, -72, -59, -16, 98, -70, 57, -47, -33, 120, -91, -9, 83, 76, 84, 30, -52, 31, -96, -54, -34, 74, 45, 12, 12, -110, -97, -105, 72, 79, -38, -30, 77, -4, 91, 103, -69, 97, -13, -47, 72, -111, 114, -42, 82, -14, 68, 31, 42, 25, 68, -13, -48, -17, -98, 72, -62, -89, -43, -7, 59, -65, -106, -111, -34, 100, -19, 32, -60, -48, 93, -49, -49, -85, 120, 24, 117, 78, -61, -34, -110, -55, 62, 65, 114, -99, -7, 5, -44, 58, -1, -109, -49, -5, 3, 
  -86, 108, 77, 25, 0, 4, -8, -53, -10, 42, 9, -93, -64, -103, -73, 62, 89, 63, -96, -10, -52, -104, -50, 96, 80, 51, -21, 19, 125, 117, 14, -17, -6, 81, 35, 38, -6, -35, -90, 53, 37, 123, -64, -85, 58, 42, 66, 90, 120, 36, 111, 100, -16, 39, -25, 21, -95, -13, 12, -48, -48, -36, -23, 118, -24, -35, -13, 46, 11, 123, -41, -88, 84, -14, 34, -107, 102, 35, 71, 122, 64, 124, 52, -110, 47, 64, 9, -78, 52, -26, -107, 103, 108, 72, -63, -54, 107, 87, 127, 87, 
  -49, 11, -110, -99, 111, -46, 25, 90, -65, 14, 15, 56, 30, -32, -15, -31, 10, -15, -59, 101, -106, -30, 72, -94, 111, -10, -81, 122, 75, 67, 76, 26, 26, 32, -104, -107, 114, 25, -94, 26, -89, 81, -87, 97, 119, 114, 79, 109, 18, -83, -46, 85, -51, 113, 119, -76, -7, -31, 52, -94, 96, -26, 84, 97, 108, -32, -39, -47, -8, 29, 94, 86, -112, -103, 62, 96, -96, -36, 80, -108, 86, -115, -47, -114, 90, -3, 112, -100, -52, -80, 15, -87, -93, 100, 121, -61, 79, -85, 74, 73, 
  -82, -26, 96, 105, 72, 45, -4, -83, -34, -31, -103, -82, 89, -13, -54, 83, -49, -21, -101, -21, 75, -106, -37, 118, -78, -102, 90, -33, 22, -8, 66, 45, 98, 11, 51, 77, 51, -16, 26, 72, -43, -120, 34, -49, 114, 93, -92, 54, 63, 8, 10, -102, -87, -55, 45, -88, 15, 47, -115, 36, 11, -32, 118, -98, 88, 44, -92, 106, -88, 67, 39, -64, -8, -100, 107, -75, -70, -70, 65, -66, -93, 63, -54, -110, -118, -64, 3, 28, -59, 22, -65, 110, 24, -61, 22, 104, 16, 116, -23, 26, 
  -39, 51, 80, 53, 21, -7, -2, -107, 103, 106, -33, 88, -11, -39, 97, 36, 24, -116, 31, -21, -41, 65, 101, -110, 78, 109, -6, 108, -111, -123, -118, -90, 118, 59, 35, 12, 93, -60, 60, -31, -98, -54, 109, 42, -73, 14, -20, 83, -58, -125, 100, 81, -41, -40, -46, -1, 97, -111, 16, -47, 28, -91, 31, -94, -70, 94, 111, 79, 13, -75, 87, -73, 7, -52, -107, 76, 60, 27, -37, 93, -3, 101, -27, 19, -107, 117, -12, 95, -113, 66, -40, -5, 55, -108, 66, 95, 32, 116, 118, 104, 
};
const TfArray<2, int> tensor_dimension4 = { 2, { 20,100 } };
const TfArray<1, float> quant4_scale = { 1, { 0.0018312191823497415, } };
const TfArray<1, int> quant4_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant4 = { (TfLiteFloatArray*)&quant4_scale, (TfLiteIntArray*)&quant4_zero, 0 };
const ALIGN(8) int8_t tensor_data5[10*20] = { 
  -78, -5, 34, 20, 103, -74, -33, 20, -103, -112, -40, -68, 109, 99, -35, 0, -107, 17, -37, 15, 
  -88, 11, -70, 40, 29, 3, 77, -54, -44, -89, 11, -95, -29, -37, -101, -113, -58, 72, 51, -105, 
  -91, 95, 40, -52, 84, 2, 57, -44, 103, 99, -62, -50, 44, -112, 49, -31, -22, 126, -98, -31, 
  -75, -23, -74, -100, -18, 62, 83, 91, 106, -92, 94, 46, 2, -79, -29, 126, 32, 40, 32, -82, 
  16, 12, -61, -88, -124, 103, -45, -7, -75, 118, 11, -122, -106, 42, -14, 28, 20, -112, 124, 27, 
  72, 98, -98, 10, -87, -26, 1, -29, 113, -117, -127, 114, -96, 54, -63, 95, -108, -13, -106, 84, 
  95, -41, 10, 93, 73, -36, 12, 53, 49, 102, -63, 70, -125, -13, -12, -13, 46, -28, 41, -58, 
  -64, 110, -62, -125, -22, -17, -33, -97, 109, 84, 82, 115, -17, -4, -21, -46, -68, 23, -47, 57, 
  -86, 76, 4, 53, 111, -6, 66, -121, -29, -91, 47, 87, 105, 119, -24, 56, -99, 108, -6, 2, 
  -31, 33, 19, 86, -86, 75, 89, -65, 96, -40, -31, 39, -98, -38, -101, 26, 123, 117, -47, 117, 
};
const TfArray<2, int> tensor_dimension5 = { 2, { 10,20 } };
const TfArray<1, float> quant5_scale = { 1, { 0.0035129685420542955, } };
const TfArray<1, int> quant5_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant5 = { (TfLiteFloatArray*)&quant5_scale, (TfLiteIntArray*)&quant5_zero, 0 };
const ALIGN(8) int8_t tensor_data6[2*10] = { 
  -36, -127, -6, -112, 8, -82, 57, 79, 2, 48, 
  72, -108, -17, 104, 99, 65, 56, -5, 28, -4, 
};
const TfArray<2, int> tensor_dimension6 = { 2, { 2,10 } };
const TfArray<1, float> quant6_scale = { 1, { 0.0056440974585711956, } };
const TfArray<1, int> quant6_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant6 = { (TfLiteFloatArray*)&quant6_scale, (TfLiteIntArray*)&quant6_zero, 0 };
const TfArray<2, int> tensor_dimension7 = { 2, { 1,20 } };
const TfArray<1, float> quant7_scale = { 1, { 0.24486921727657318, } };
const TfArray<1, int> quant7_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant7 = { (TfLiteFloatArray*)&quant7_scale, (TfLiteIntArray*)&quant7_zero, 0 };
const TfArray<2, int> tensor_dimension8 = { 2, { 1,10 } };
const TfArray<1, float> quant8_scale = { 1, { 0.20669673383235931, } };
const TfArray<1, int> quant8_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant8 = { (TfLiteFloatArray*)&quant8_scale, (TfLiteIntArray*)&quant8_zero, 0 };
const TfArray<2, int> tensor_dimension9 = { 2, { 1,2 } };
const TfArray<1, float> quant9_scale = { 1, { 0.14228315651416779, } };
const TfArray<1, int> quant9_zero = { 1, { -69 } };
const TfLiteAffineQuantization quant9 = { (TfLiteFloatArray*)&quant9_scale, (TfLiteIntArray*)&quant9_zero, 0 };
const TfArray<2, int> tensor_dimension10 = { 2, { 1,2 } };
const TfArray<1, float> quant10_scale = { 1, { 0.00390625, } };
const TfArray<1, int> quant10_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant10 = { (TfLiteFloatArray*)&quant10_scale, (TfLiteIntArray*)&quant10_zero, 0 };
const TfLiteFullyConnectedParams opdata0 = { kTfLiteActRelu, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs0 = { 3, { 0,4,1 } };
const TfArray<1, int> outputs0 = { 1, { 7 } };
const TfLiteFullyConnectedParams opdata1 = { kTfLiteActRelu, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs1 = { 3, { 7,5,2 } };
const TfArray<1, int> outputs1 = { 1, { 8 } };
const TfLiteFullyConnectedParams opdata2 = { kTfLiteActNone, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs2 = { 3, { 8,6,3 } };
const TfArray<1, int> outputs2 = { 1, { 9 } };
const TfLiteSoftmaxParams opdata3 = { 1 };
const TfArray<1, int> inputs3 = { 1, { 9 } };
const TfArray<1, int> outputs3 = { 1, { 10 } };
const TensorInfo_t tensorData[] = {
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension0, 100, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant0))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data1, (TfLiteIntArray*)&tensor_dimension1, 80, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant1))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data2, (TfLiteIntArray*)&tensor_dimension2, 40, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant2))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data3, (TfLiteIntArray*)&tensor_dimension3, 8, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant3))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data4, (TfLiteIntArray*)&tensor_dimension4, 2000, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant4))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data5, (TfLiteIntArray*)&tensor_dimension5, 200, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant5))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data6, (TfLiteIntArray*)&tensor_dimension6, 20, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant6))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 112, (TfLiteIntArray*)&tensor_dimension7, 20, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant7))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension8, 10, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant8))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 16, (TfLiteIntArray*)&tensor_dimension9, 2, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant9))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension10, 2, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant10))}, },
};const NodeInfo_t nodeData[] = {
  { (TfLiteIntArray*)&inputs0, (TfLiteIntArray*)&outputs0, const_cast<void*>(static_cast<const void*>(&opdata0)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs1, (TfLiteIntArray*)&outputs1, const_cast<void*>(static_cast<const void*>(&opdata1)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs2, (TfLiteIntArray*)&outputs2, const_cast<void*>(static_cast<const void*>(&opdata2)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs3, (TfLiteIntArray*)&outputs3, const_cast<void*>(static_cast<const void*>(&opdata3)), OP_SOFTMAX, },
};
static std::vector<void*> overflow_buffers;
static TfLiteStatus AllocatePersistentBuffer(struct TfLiteContext* ctx,
                                                 size_t bytes, void** ptr) {
  if (current_location - bytes < tensor_boundary) {
    // OK, this will look super weird, but.... we have CMSIS-NN buffers which
    // we cannot calculate beforehand easily.
    *ptr = malloc(bytes);
    if (*ptr == NULL) {
      printf("ERR: Failed to allocate persistent buffer of size %d\n", (int)bytes);
      return kTfLiteError;
    }
    overflow_buffers.push_back(*ptr);
    return kTfLiteOk;
  }

  current_location -= bytes;

  *ptr = current_location;
  return kTfLiteOk;
}
typedef struct {
  size_t bytes;
  void *ptr;
} scratch_buffer_t;
static std::vector<scratch_buffer_t> scratch_buffers;

static TfLiteStatus RequestScratchBufferInArena(struct TfLiteContext* ctx, size_t bytes,
                                                int* buffer_idx) {
  scratch_buffer_t b;
  b.bytes = bytes;

  TfLiteStatus s = AllocatePersistentBuffer(ctx, b.bytes, &b.ptr);
  if (s != kTfLiteOk) {
    return s;
  }

  scratch_buffers.push_back(b);

  *buffer_idx = scratch_buffers.size() - 1;

  return kTfLiteOk;
}

static void* GetScratchBuffer(struct TfLiteContext* ctx, int buffer_idx) {
  if (buffer_idx > static_cast<int>(scratch_buffers.size()) - 1) {
    return NULL;
  }
  return scratch_buffers[buffer_idx].ptr;
}
} // namespace

  TfLiteStatus trained_model_init( void*(*alloc_fnc)(size_t,size_t) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  tensor_arena = (uint8_t*) alloc_fnc(16, kTensorArenaSize);
  if (!tensor_arena) {
    printf("ERR: failed to allocate tensor arena\n");
    return kTfLiteError;
  }
#endif
  tensor_boundary = tensor_arena;
  current_location = tensor_arena + kTensorArenaSize;
  ctx.AllocatePersistentBuffer = &AllocatePersistentBuffer;
  ctx.RequestScratchBufferInArena = &RequestScratchBufferInArena;
  ctx.GetScratchBuffer = &GetScratchBuffer;
  ctx.tensors = tflTensors;
  ctx.tensors_size = 11;
  for(size_t i = 0; i < 11; ++i) {
    tflTensors[i].type = tensorData[i].type;
    tflTensors[i].is_variable = 0;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    tflTensors[i].allocation_type = tensorData[i].allocation_type;
#else
    tflTensors[i].allocation_type = (tensor_arena <= tensorData[i].data && tensorData[i].data < tensor_arena + kTensorArenaSize) ? kTfLiteArenaRw : kTfLiteMmapRo;
#endif
    tflTensors[i].bytes = tensorData[i].bytes;
    tflTensors[i].dims = tensorData[i].dims;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    if(tflTensors[i].allocation_type == kTfLiteArenaRw){
      uint8_t* start = (uint8_t*) ((uintptr_t)tensorData[i].data + (uintptr_t) tensor_arena);

     tflTensors[i].data.data =  start;
    }
    else{
       tflTensors[i].data.data = tensorData[i].data;
    }
#else
    tflTensors[i].data.data = tensorData[i].data;
#endif // EI_CLASSIFIER_ALLOCATION_HEAP
    tflTensors[i].quantization = tensorData[i].quantization;
    if (tflTensors[i].quantization.type == kTfLiteAffineQuantization) {
      TfLiteAffineQuantization const* quant = ((TfLiteAffineQuantization const*)(tensorData[i].quantization.params));
      tflTensors[i].params.scale = quant->scale->data[0];
      tflTensors[i].params.zero_point = quant->zero_point->data[0];
    }
    if (tflTensors[i].allocation_type == kTfLiteArenaRw) {
      auto data_end_ptr = (uint8_t*)tflTensors[i].data.data + tensorData[i].bytes;
      if (data_end_ptr > tensor_boundary) {
        tensor_boundary = data_end_ptr;
      }
    }
  }
  if (tensor_boundary > current_location /* end of arena size */) {
    printf("ERR: tensor arena is too small, does not fit model - even without scratch buffers\n");
    return kTfLiteError;
  }
  registrations[OP_FULLY_CONNECTED] = *tflite::ops::micro::Register_FULLY_CONNECTED();
  registrations[OP_SOFTMAX] = *tflite::ops::micro::Register_SOFTMAX();

  for(size_t i = 0; i < 4; ++i) {
    tflNodes[i].inputs = nodeData[i].inputs;
    tflNodes[i].outputs = nodeData[i].outputs;
    tflNodes[i].builtin_data = nodeData[i].builtin_data;
    tflNodes[i].custom_initial_data = nullptr;
    tflNodes[i].custom_initial_data_size = 0;
    if (registrations[nodeData[i].used_op_index].init) {
      tflNodes[i].user_data = registrations[nodeData[i].used_op_index].init(&ctx, (const char*)tflNodes[i].builtin_data, 0);
    }
  }
  for(size_t i = 0; i < 4; ++i) {
    if (registrations[nodeData[i].used_op_index].prepare) {
      TfLiteStatus status = registrations[nodeData[i].used_op_index].prepare(&ctx, &tflNodes[i]);
      if (status != kTfLiteOk) {
        return status;
      }
    }
  }
  return kTfLiteOk;
}

static const int inTensorIndices[] = {
  0, 
};
TfLiteTensor* trained_model_input(int index) {
  return &ctx.tensors[inTensorIndices[index]];
}

static const int outTensorIndices[] = {
  10, 
};
TfLiteTensor* trained_model_output(int index) {
  return &ctx.tensors[outTensorIndices[index]];
}

TfLiteStatus trained_model_invoke() {
  for(size_t i = 0; i < 4; ++i) {
    TfLiteStatus status = registrations[nodeData[i].used_op_index].invoke(&ctx, &tflNodes[i]);

#if EI_CLASSIFIER_PRINT_STATE
    ei_printf("layer %lu\n", i);
    ei_printf("    inputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].inputs->size; ix++) {
      auto d = tensorData[tflNodes[i].inputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");

    ei_printf("    outputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].outputs->size; ix++) {
      auto d = tensorData[tflNodes[i].outputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");
#endif // EI_CLASSIFIER_PRINT_STATE

    if (status != kTfLiteOk) {
      return status;
    }
  }
  return kTfLiteOk;
}

TfLiteStatus trained_model_reset( void (*free_fnc)(void* ptr) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  free_fnc(tensor_arena);
#endif
  scratch_buffers.clear();
  for (size_t ix = 0; ix < overflow_buffers.size(); ix++) {
    free(overflow_buffers[ix]);
  }
  overflow_buffers.clear();
  return kTfLiteOk;
}