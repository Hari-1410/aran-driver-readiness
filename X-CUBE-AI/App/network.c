/**
  ******************************************************************************
  * @file    network.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-07-04T23:27:17+0530
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */


#include "network.h"
#include "network_data.h"

#include "ai_platform.h"
#include "ai_platform_interface.h"
#include "ai_math_helpers.h"

#include "core_common.h"
#include "core_convert.h"

#include "layers.h"



#undef AI_NET_OBJ_INSTANCE
#define AI_NET_OBJ_INSTANCE g_network
 
#undef AI_NETWORK_MODEL_SIGNATURE
#define AI_NETWORK_MODEL_SIGNATURE     "0xda3fac54a2c51570f406d274d1e87d11"

#ifndef AI_TOOLS_REVISION_ID
#define AI_TOOLS_REVISION_ID     ""
#endif

#undef AI_TOOLS_DATE_TIME
#define AI_TOOLS_DATE_TIME   "2026-07-04T23:27:17+0530"

#undef AI_TOOLS_COMPILE_TIME
#define AI_TOOLS_COMPILE_TIME    __DATE__ " " __TIME__

#undef AI_NETWORK_N_BATCHES
#define AI_NETWORK_N_BATCHES         (1)

static ai_ptr g_network_activations_map[1] = AI_C_ARRAY_INIT;
static ai_ptr g_network_weights_map[1] = AI_C_ARRAY_INIT;



/**  Array declarations section  **********************************************/
/* Array#0 */
AI_ARRAY_OBJ_DECLARE(
  serving_default_keras_tensor0_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 2500, AI_STATIC)

/* Array#1 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 9944, AI_STATIC)

/* Array#2 */
AI_ARRAY_OBJ_DECLARE(
  pool_4_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2480, AI_STATIC)

/* Array#3 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 4864, AI_STATIC)

/* Array#4 */
AI_ARRAY_OBJ_DECLARE(
  pool_10_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1216, AI_STATIC)

/* Array#5 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_13_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2368, AI_STATIC)

/* Array#6 */
AI_ARRAY_OBJ_DECLARE(
  pool_15_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 32, AI_STATIC)

/* Array#7 */
AI_ARRAY_OBJ_DECLARE(
  gemm_16_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#8 */
AI_ARRAY_OBJ_DECLARE(
  gemm_17_output_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1, AI_STATIC)

/* Array#9 */
AI_ARRAY_OBJ_DECLARE(
  nl_18_output_array, AI_ARRAY_FORMAT_S8|AI_FMT_FLAG_IS_IO,
  NULL, NULL, 1, AI_STATIC)

/* Array#10 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 120, AI_STATIC)

/* Array#11 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 8, AI_STATIC)

/* Array#12 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 896, AI_STATIC)

/* Array#13 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16, AI_STATIC)

/* Array#14 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_13_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 1536, AI_STATIC)

/* Array#15 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_13_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 32, AI_STATIC)

/* Array#16 */
AI_ARRAY_OBJ_DECLARE(
  gemm_16_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 512, AI_STATIC)

/* Array#17 */
AI_ARRAY_OBJ_DECLARE(
  gemm_16_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 16, AI_STATIC)

/* Array#18 */
AI_ARRAY_OBJ_DECLARE(
  gemm_17_weights_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 16, AI_STATIC)

/* Array#19 */
AI_ARRAY_OBJ_DECLARE(
  gemm_17_bias_array, AI_ARRAY_FORMAT_S32,
  NULL, NULL, 1, AI_STATIC)

/* Array#20 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_1_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 412, AI_STATIC)

/* Array#21 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_7_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 2240, AI_STATIC)

/* Array#22 */
AI_ARRAY_OBJ_DECLARE(
  conv2d_13_scratch0_array, AI_ARRAY_FORMAT_S8,
  NULL, NULL, 3712, AI_STATIC)

/* Array#23 */
AI_ARRAY_OBJ_DECLARE(
  gemm_16_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 112, AI_STATIC)

/* Array#24 */
AI_ARRAY_OBJ_DECLARE(
  gemm_17_scratch0_array, AI_ARRAY_FORMAT_S16,
  NULL, NULL, 16, AI_STATIC)

/**  Array metadata declarations section  *************************************/
/* Int quant #0 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_13_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.20477555692195892f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #1 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_13_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 32,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.004573691636323929f, 0.003630192019045353f, 0.0023149920161813498f, 0.0036867093294858932f, 0.0026899713557213545f, 0.002002871595323086f, 0.001791894668713212f, 0.002933777868747711f, 0.002816898049786687f, 0.0025254494976252317f, 0.00332795106805861f, 0.0021231903228908777f, 0.003130501601845026f, 0.003458554157987237f, 0.0016639078967273235f, 0.0019933907315135f, 0.0034408115316182375f, 0.0026680806186050177f, 0.0039060681592673063f, 0.0032583961728960276f, 0.002243140945211053f, 0.0038541757967323065f, 0.002194611355662346f, 0.004359820391982794f, 0.0018187938258051872f, 0.0033775523770600557f, 0.0037161153741180897f, 0.003683661110699177f, 0.0022334163077175617f, 0.0032964928541332483f, 0.0036444924771785736f, 0.0021230808924883604f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #2 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_1_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.1311100721359253f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #3 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_1_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 8,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.002679978497326374f, 0.0028719380497932434f, 0.002886665752157569f, 0.002850544173270464f, 0.003256413619965315f, 0.0026408934500068426f, 0.0031218479853123426f, 0.002534608356654644f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #4 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_7_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.24009500443935394f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #5 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(conv2d_7_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0044390056282281876f, 0.002898850478231907f, 0.005009970162063837f, 0.004262277856469154f, 0.002307215239852667f, 0.0023887213319540024f, 0.003309104824438691f, 0.003402802860364318f, 0.003025603713467717f, 0.0037211019080132246f, 0.003196920035406947f, 0.0025401355233043432f, 0.003495229175314307f, 0.0035382509231567383f, 0.004229225683957338f, 0.003943116404116154f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #6 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_16_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.054811667650938034f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #7 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_16_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 16,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.005924438126385212f, 0.005426843650639057f, 0.004046546760946512f, 0.0028681501280516386f, 0.00399024086073041f, 0.00332415453158319f, 0.00273964274674654f, 0.009864017367362976f, 0.005216455552726984f, 0.005593339912593365f, 0.00292684487067163f, 0.0048896572552621365f, 0.0028783283196389675f, 0.005810766946524382f, 0.002544812858104706f, 0.005523317027837038f),
    AI_PACK_INTQ_ZP(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)))

/* Int quant #8 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_17_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.1365252435207367f),
    AI_PACK_INTQ_ZP(-77)))

/* Int quant #9 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(gemm_17_weights_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.006428131368011236f),
    AI_PACK_INTQ_ZP(0)))

/* Int quant #10 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(nl_18_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.00390625f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #11 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_10_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.24009500443935394f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #12 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_15_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.0335148423910141f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #13 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(pool_4_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.1311100721359253f),
    AI_PACK_INTQ_ZP(-128)))

/* Int quant #14 */
AI_INTQ_INFO_LIST_OBJ_DECLARE(serving_default_keras_tensor0_output_array_intq, AI_STATIC_CONST,
  AI_BUFFER_META_FLAG_SCALE_FLOAT|AI_BUFFER_META_FLAG_ZEROPOINT_S8, 1,
  AI_PACK_INTQ_INFO(
    AI_PACK_INTQ_SCALE(0.14895595610141754f),
    AI_PACK_INTQ_ZP(-48)))

/**  Tensor declarations section  *********************************************/
/* Tensor #0 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_13_bias, AI_STATIC,
  0, 0x0,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 4, 4, 128, 128),
  1, &conv2d_13_bias_array, NULL)

/* Tensor #1 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_13_output, AI_STATIC,
  1, 0x1,
  AI_SHAPE_INIT(4, 1, 32, 74, 1), AI_STRIDE_INIT(4, 1, 1, 32, 2368),
  1, &conv2d_13_output_array, &conv2d_13_output_array_intq)

/* Tensor #2 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_13_output0, AI_STATIC,
  2, 0x1,
  AI_SHAPE_INIT(4, 1, 32, 1, 74), AI_STRIDE_INIT(4, 1, 1, 32, 32),
  1, &conv2d_13_output_array, &conv2d_13_output_array_intq)

/* Tensor #3 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_13_scratch0, AI_STATIC,
  3, 0x0,
  AI_SHAPE_INIT(4, 1, 3712, 1, 1), AI_STRIDE_INIT(4, 1, 1, 3712, 3712),
  1, &conv2d_13_scratch0_array, NULL)

/* Tensor #4 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_13_weights, AI_STATIC,
  4, 0x1,
  AI_SHAPE_INIT(4, 16, 3, 1, 32), AI_STRIDE_INIT(4, 1, 16, 512, 1536),
  1, &conv2d_13_weights_array, &conv2d_13_weights_array_intq)

/* Tensor #5 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_bias, AI_STATIC,
  5, 0x0,
  AI_SHAPE_INIT(4, 1, 8, 1, 1), AI_STRIDE_INIT(4, 4, 4, 32, 32),
  1, &conv2d_1_bias_array, NULL)

/* Tensor #6 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_output, AI_STATIC,
  6, 0x1,
  AI_SHAPE_INIT(4, 1, 8, 1243, 1), AI_STRIDE_INIT(4, 1, 1, 8, 9944),
  1, &conv2d_1_output_array, &conv2d_1_output_array_intq)

/* Tensor #7 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_scratch0, AI_STATIC,
  7, 0x0,
  AI_SHAPE_INIT(4, 1, 412, 1, 1), AI_STRIDE_INIT(4, 1, 1, 412, 412),
  1, &conv2d_1_scratch0_array, NULL)

/* Tensor #8 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_1_weights, AI_STATIC,
  8, 0x1,
  AI_SHAPE_INIT(4, 1, 15, 1, 8), AI_STRIDE_INIT(4, 1, 1, 8, 120),
  1, &conv2d_1_weights_array, &conv2d_1_weights_array_intq)

/* Tensor #9 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_bias, AI_STATIC,
  9, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &conv2d_7_bias_array, NULL)

/* Tensor #10 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_output, AI_STATIC,
  10, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 304, 1), AI_STRIDE_INIT(4, 1, 1, 16, 4864),
  1, &conv2d_7_output_array, &conv2d_7_output_array_intq)

/* Tensor #11 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_scratch0, AI_STATIC,
  11, 0x0,
  AI_SHAPE_INIT(4, 1, 2240, 1, 1), AI_STRIDE_INIT(4, 1, 1, 2240, 2240),
  1, &conv2d_7_scratch0_array, NULL)

/* Tensor #12 */
AI_TENSOR_OBJ_DECLARE(
  conv2d_7_weights, AI_STATIC,
  12, 0x1,
  AI_SHAPE_INIT(4, 8, 7, 1, 16), AI_STRIDE_INIT(4, 1, 8, 128, 896),
  1, &conv2d_7_weights_array, &conv2d_7_weights_array_intq)

/* Tensor #13 */
AI_TENSOR_OBJ_DECLARE(
  gemm_16_bias, AI_STATIC,
  13, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 4, 4, 64, 64),
  1, &gemm_16_bias_array, NULL)

/* Tensor #14 */
AI_TENSOR_OBJ_DECLARE(
  gemm_16_output, AI_STATIC,
  14, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 1, 1, 16, 16),
  1, &gemm_16_output_array, &gemm_16_output_array_intq)

/* Tensor #15 */
AI_TENSOR_OBJ_DECLARE(
  gemm_16_scratch0, AI_STATIC,
  15, 0x0,
  AI_SHAPE_INIT(4, 1, 112, 1, 1), AI_STRIDE_INIT(4, 2, 2, 224, 224),
  1, &gemm_16_scratch0_array, NULL)

/* Tensor #16 */
AI_TENSOR_OBJ_DECLARE(
  gemm_16_weights, AI_STATIC,
  16, 0x1,
  AI_SHAPE_INIT(4, 32, 16, 1, 1), AI_STRIDE_INIT(4, 1, 32, 512, 512),
  1, &gemm_16_weights_array, &gemm_16_weights_array_intq)

/* Tensor #17 */
AI_TENSOR_OBJ_DECLARE(
  gemm_17_bias, AI_STATIC,
  17, 0x0,
  AI_SHAPE_INIT(4, 1, 1, 1, 1), AI_STRIDE_INIT(4, 4, 4, 4, 4),
  1, &gemm_17_bias_array, NULL)

/* Tensor #18 */
AI_TENSOR_OBJ_DECLARE(
  gemm_17_output, AI_STATIC,
  18, 0x1,
  AI_SHAPE_INIT(4, 1, 1, 1, 1), AI_STRIDE_INIT(4, 1, 1, 1, 1),
  1, &gemm_17_output_array, &gemm_17_output_array_intq)

/* Tensor #19 */
AI_TENSOR_OBJ_DECLARE(
  gemm_17_scratch0, AI_STATIC,
  19, 0x0,
  AI_SHAPE_INIT(4, 1, 16, 1, 1), AI_STRIDE_INIT(4, 2, 2, 32, 32),
  1, &gemm_17_scratch0_array, NULL)

/* Tensor #20 */
AI_TENSOR_OBJ_DECLARE(
  gemm_17_weights, AI_STATIC,
  20, 0x1,
  AI_SHAPE_INIT(4, 16, 1, 1, 1), AI_STRIDE_INIT(4, 1, 16, 16, 16),
  1, &gemm_17_weights_array, &gemm_17_weights_array_intq)

/* Tensor #21 */
AI_TENSOR_OBJ_DECLARE(
  nl_18_output, AI_STATIC,
  21, 0x1,
  AI_SHAPE_INIT(4, 1, 1, 1, 1), AI_STRIDE_INIT(4, 1, 1, 1, 1),
  1, &nl_18_output_array, &nl_18_output_array_intq)

/* Tensor #22 */
AI_TENSOR_OBJ_DECLARE(
  pool_10_output, AI_STATIC,
  22, 0x1,
  AI_SHAPE_INIT(4, 1, 16, 76, 1), AI_STRIDE_INIT(4, 1, 1, 16, 1216),
  1, &pool_10_output_array, &pool_10_output_array_intq)

/* Tensor #23 */
AI_TENSOR_OBJ_DECLARE(
  pool_15_output, AI_STATIC,
  23, 0x1,
  AI_SHAPE_INIT(4, 1, 32, 1, 1), AI_STRIDE_INIT(4, 1, 1, 32, 32),
  1, &pool_15_output_array, &pool_15_output_array_intq)

/* Tensor #24 */
AI_TENSOR_OBJ_DECLARE(
  pool_4_output, AI_STATIC,
  24, 0x1,
  AI_SHAPE_INIT(4, 1, 8, 310, 1), AI_STRIDE_INIT(4, 1, 1, 8, 2480),
  1, &pool_4_output_array, &pool_4_output_array_intq)

/* Tensor #25 */
AI_TENSOR_OBJ_DECLARE(
  serving_default_keras_tensor0_output, AI_STATIC,
  25, 0x1,
  AI_SHAPE_INIT(4, 1, 1, 1, 2500), AI_STRIDE_INIT(4, 1, 1, 1, 1),
  1, &serving_default_keras_tensor0_output_array, &serving_default_keras_tensor0_output_array_intq)

/* Tensor #26 */
AI_TENSOR_OBJ_DECLARE(
  serving_default_keras_tensor0_output0, AI_STATIC,
  26, 0x1,
  AI_SHAPE_INIT(4, 1, 1, 2500, 1), AI_STRIDE_INIT(4, 1, 1, 1, 2500),
  1, &serving_default_keras_tensor0_output_array, &serving_default_keras_tensor0_output_array_intq)



/**  Layer declarations section  **********************************************/



AI_STATIC_CONST ai_i8 nl_18_nl_params_data[] = { -128, -128, -128, -128, -128, -128, -127, -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -126, -125, -125, -124, -124, -123, -123, -122, -121, -120, -119, -117, -116, -114, -112, -110, -108, -105, -102, -99, -95, -91, -86, -81, -76, -70, -64, -57, -50, -42, -34, -26, -17, -9, 0, 9, 17, 26, 34, 42, 50, 57, 64, 70, 76, 81, 86, 91, 95, 99, 102, 105, 108, 110, 112, 114, 116, 117, 119, 120, 121, 122, 123, 123, 124, 124, 125, 125, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };
AI_ARRAY_OBJ_DECLARE(
    nl_18_nl_params, AI_ARRAY_FORMAT_S8,
    nl_18_nl_params_data, nl_18_nl_params_data, 256, AI_STATIC_CONST)
AI_TENSOR_CHAIN_OBJ_DECLARE(
  nl_18_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &nl_18_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  nl_18_layer, 18,
  NL_TYPE, 0x0, NULL,
  nl, forward_nl_integer,
  &nl_18_chain,
  NULL, &nl_18_layer, AI_STATIC, 
  .nl_params = &nl_18_nl_params, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_17_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_16_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_17_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_17_weights, &gemm_17_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_17_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_17_layer, 17,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA,
  &gemm_17_chain,
  NULL, &nl_18_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  gemm_16_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_15_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_16_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 2, &gemm_16_weights, &gemm_16_bias),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &gemm_16_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  gemm_16_layer, 16,
  DENSE_TYPE, 0x0, NULL,
  dense, forward_dense_integer_SSSA_ch,
  &gemm_16_chain,
  NULL, &gemm_17_layer, AI_STATIC, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_15_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_13_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_15_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_15_layer, 15,
  POOL_TYPE, 0x0, NULL,
  pool, forward_ap_integer_INT8,
  &pool_15_chain,
  NULL, &gemm_16_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(1, 74), 
  .pool_stride = AI_SHAPE_2D_INIT(1, 74), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_13_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_10_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_13_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_13_weights, &conv2d_13_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_13_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_13_layer, 13,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_deep_sssa8_ch,
  &conv2d_13_chain,
  NULL, &pool_15_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_10_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_7_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_10_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_10_layer, 10,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &pool_10_chain,
  NULL, &conv2d_13_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(4, 1), 
  .pool_stride = AI_SHAPE_2D_INIT(4, 1), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_7_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_4_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_7_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_7_weights, &conv2d_7_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_7_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_7_layer, 7,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_sssa8_ch,
  &conv2d_7_chain,
  NULL, &pool_10_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(1, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  pool_4_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &pool_4_output),
  AI_TENSOR_LIST_OBJ_EMPTY,
  AI_TENSOR_LIST_OBJ_EMPTY
)

AI_LAYER_OBJ_DECLARE(
  pool_4_layer, 4,
  POOL_TYPE, 0x0, NULL,
  pool, forward_mp_integer_INT8,
  &pool_4_chain,
  NULL, &conv2d_7_layer, AI_STATIC, 
  .pool_size = AI_SHAPE_2D_INIT(4, 1), 
  .pool_stride = AI_SHAPE_2D_INIT(4, 1), 
  .pool_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
)

AI_TENSOR_CHAIN_OBJ_DECLARE(
  conv2d_1_chain, AI_STATIC_CONST, 4,
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &serving_default_keras_tensor0_output0),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_output),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 3, &conv2d_1_weights, &conv2d_1_bias, NULL),
  AI_TENSOR_LIST_OBJ_INIT(AI_FLAG_NONE, 1, &conv2d_1_scratch0)
)

AI_LAYER_OBJ_DECLARE(
  conv2d_1_layer, 1,
  CONV2D_TYPE, 0x0, NULL,
  conv2d, forward_conv2d_sssa8_ch,
  &conv2d_1_chain,
  NULL, &pool_4_layer, AI_STATIC, 
  .groups = 1, 
  .filter_stride = AI_SHAPE_2D_INIT(2, 1), 
  .dilation = AI_SHAPE_2D_INIT(1, 1), 
  .filter_pad = AI_SHAPE_INIT(4, 0, 0, 0, 0), 
  .in_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
  .out_ch_format = AI_LAYER_FORMAT_CHANNEL_LAST_VALID, 
)


#if (AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 3372, 1, 1),
    3372, NULL, NULL),
  AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
    AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 12856, 1, 1),
    12856, NULL, NULL),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &serving_default_keras_tensor0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &nl_18_output),
  &conv2d_1_layer, 0x8c75a781, NULL)

#else

AI_NETWORK_OBJ_DECLARE(
  AI_NET_OBJ_INSTANCE, AI_STATIC,
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 3372, 1, 1),
      3372, NULL, NULL)
  ),
  AI_BUFFER_ARRAY_OBJ_INIT_STATIC(
  	AI_FLAG_NONE, 1,
    AI_BUFFER_INIT(AI_FLAG_NONE,  AI_BUFFER_FORMAT_U8,
      AI_BUFFER_SHAPE_INIT(AI_SHAPE_BCWH, 4, 1, 12856, 1, 1),
      12856, NULL, NULL)
  ),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_IN_NUM, &serving_default_keras_tensor0_output),
  AI_TENSOR_LIST_IO_OBJ_INIT(AI_FLAG_NONE, AI_NETWORK_OUT_NUM, &nl_18_output),
  &conv2d_1_layer, 0x8c75a781, NULL)

#endif	/*(AI_TOOLS_API_VERSION < AI_TOOLS_API_VERSION_1_5)*/



/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_activations(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_activations_map(g_network_activations_map, 1, params)) {
    /* Updating activations (byte) offsets */
    
    serving_default_keras_tensor0_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    serving_default_keras_tensor0_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    conv2d_1_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 2500);
    conv2d_1_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 2500);
    conv2d_1_output_array.data = AI_PTR(g_network_activations_map[0] + 2912);
    conv2d_1_output_array.data_start = AI_PTR(g_network_activations_map[0] + 2912);
    pool_4_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    pool_4_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    conv2d_7_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 2480);
    conv2d_7_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 2480);
    conv2d_7_output_array.data = AI_PTR(g_network_activations_map[0] + 4720);
    conv2d_7_output_array.data_start = AI_PTR(g_network_activations_map[0] + 4720);
    pool_10_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    pool_10_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    conv2d_13_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 1216);
    conv2d_13_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 1216);
    conv2d_13_output_array.data = AI_PTR(g_network_activations_map[0] + 4928);
    conv2d_13_output_array.data_start = AI_PTR(g_network_activations_map[0] + 4928);
    pool_15_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    pool_15_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    gemm_16_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 32);
    gemm_16_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 32);
    gemm_16_output_array.data = AI_PTR(g_network_activations_map[0] + 256);
    gemm_16_output_array.data_start = AI_PTR(g_network_activations_map[0] + 256);
    gemm_17_scratch0_array.data = AI_PTR(g_network_activations_map[0] + 0);
    gemm_17_scratch0_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    gemm_17_output_array.data = AI_PTR(g_network_activations_map[0] + 32);
    gemm_17_output_array.data_start = AI_PTR(g_network_activations_map[0] + 32);
    nl_18_output_array.data = AI_PTR(g_network_activations_map[0] + 0);
    nl_18_output_array.data_start = AI_PTR(g_network_activations_map[0] + 0);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_ACTIVATIONS);
  return false;
}




/******************************************************************************/
AI_DECLARE_STATIC
ai_bool network_configure_weights(
  ai_network* net_ctx, const ai_network_params* params)
{
  AI_ASSERT(net_ctx)

  if (ai_platform_get_weights_map(g_network_weights_map, 1, params)) {
    /* Updating weights (byte) offsets */
    
    conv2d_1_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_1_weights_array.data = AI_PTR(g_network_weights_map[0] + 0);
    conv2d_1_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 0);
    conv2d_1_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_1_bias_array.data = AI_PTR(g_network_weights_map[0] + 120);
    conv2d_1_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 120);
    conv2d_7_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_7_weights_array.data = AI_PTR(g_network_weights_map[0] + 152);
    conv2d_7_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 152);
    conv2d_7_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_7_bias_array.data = AI_PTR(g_network_weights_map[0] + 1048);
    conv2d_7_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 1048);
    conv2d_13_weights_array.format |= AI_FMT_FLAG_CONST;
    conv2d_13_weights_array.data = AI_PTR(g_network_weights_map[0] + 1112);
    conv2d_13_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 1112);
    conv2d_13_bias_array.format |= AI_FMT_FLAG_CONST;
    conv2d_13_bias_array.data = AI_PTR(g_network_weights_map[0] + 2648);
    conv2d_13_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 2648);
    gemm_16_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_16_weights_array.data = AI_PTR(g_network_weights_map[0] + 2776);
    gemm_16_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 2776);
    gemm_16_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_16_bias_array.data = AI_PTR(g_network_weights_map[0] + 3288);
    gemm_16_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 3288);
    gemm_17_weights_array.format |= AI_FMT_FLAG_CONST;
    gemm_17_weights_array.data = AI_PTR(g_network_weights_map[0] + 3352);
    gemm_17_weights_array.data_start = AI_PTR(g_network_weights_map[0] + 3352);
    gemm_17_bias_array.format |= AI_FMT_FLAG_CONST;
    gemm_17_bias_array.data = AI_PTR(g_network_weights_map[0] + 3368);
    gemm_17_bias_array.data_start = AI_PTR(g_network_weights_map[0] + 3368);
    return true;
  }
  AI_ERROR_TRAP(net_ctx, INIT_FAILED, NETWORK_WEIGHTS);
  return false;
}


/**  PUBLIC APIs SECTION  *****************************************************/



AI_DEPRECATED
AI_API_ENTRY
ai_bool ai_network_get_info(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 552962,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .params            = AI_STRUCT_INIT,
      .activations       = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x8c75a781,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}



AI_API_ENTRY
ai_bool ai_network_get_report(
  ai_handle network, ai_network_report* report)
{
  ai_network* net_ctx = AI_NETWORK_ACQUIRE_CTX(network);

  if (report && net_ctx)
  {
    ai_network_report r = {
      .model_name        = AI_NETWORK_MODEL_NAME,
      .model_signature   = AI_NETWORK_MODEL_SIGNATURE,
      .model_datetime    = AI_TOOLS_DATE_TIME,
      
      .compile_datetime  = AI_TOOLS_COMPILE_TIME,
      
      .runtime_revision  = ai_platform_runtime_get_revision(),
      .runtime_version   = ai_platform_runtime_get_version(),

      .tool_revision     = AI_TOOLS_REVISION_ID,
      .tool_version      = {AI_TOOLS_VERSION_MAJOR, AI_TOOLS_VERSION_MINOR,
                            AI_TOOLS_VERSION_MICRO, 0x0},
      .tool_api_version  = AI_STRUCT_INIT,

      .api_version            = ai_platform_api_get_version(),
      .interface_api_version  = ai_platform_interface_api_get_version(),
      
      .n_macc            = 552962,
      .n_inputs          = 0,
      .inputs            = NULL,
      .n_outputs         = 0,
      .outputs           = NULL,
      .map_signature     = AI_MAGIC_SIGNATURE,
      .map_weights       = AI_STRUCT_INIT,
      .map_activations   = AI_STRUCT_INIT,
      .n_nodes           = 0,
      .signature         = 0x8c75a781,
    };

    if (!ai_platform_api_get_network_report(network, &r)) return false;

    *report = r;
    return true;
  }
  return false;
}


AI_API_ENTRY
ai_error ai_network_get_error(ai_handle network)
{
  return ai_platform_network_get_error(network);
}


AI_API_ENTRY
ai_error ai_network_create(
  ai_handle* network, const ai_buffer* network_config)
{
  return ai_platform_network_create(
    network, network_config, 
    AI_CONTEXT_OBJ(&AI_NET_OBJ_INSTANCE),
    AI_TOOLS_API_VERSION_MAJOR, AI_TOOLS_API_VERSION_MINOR, AI_TOOLS_API_VERSION_MICRO);
}


AI_API_ENTRY
ai_error ai_network_create_and_init(
  ai_handle* network, const ai_handle activations[], const ai_handle weights[])
{
  ai_error err;
  ai_network_params params;

  err = ai_network_create(network, AI_NETWORK_DATA_CONFIG);
  if (err.type != AI_ERROR_NONE) {
    return err;
  }
  
  if (ai_network_data_params_get(&params) != true) {
    err = ai_network_get_error(*network);
    return err;
  }
#if defined(AI_NETWORK_DATA_ACTIVATIONS_COUNT)
  /* set the addresses of the activations buffers */
  for (ai_u16 idx=0; activations && idx<params.map_activations.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_activations, idx, activations[idx]);
  }
#endif
#if defined(AI_NETWORK_DATA_WEIGHTS_COUNT)
  /* set the addresses of the weight buffers */
  for (ai_u16 idx=0; weights && idx<params.map_weights.size; idx++) {
    AI_BUFFER_ARRAY_ITEM_SET_ADDRESS(&params.map_weights, idx, weights[idx]);
  }
#endif
  if (ai_network_init(*network, &params) != true) {
    err = ai_network_get_error(*network);
  }
  return err;
}


AI_API_ENTRY
ai_buffer* ai_network_inputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_inputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_buffer* ai_network_outputs_get(ai_handle network, ai_u16 *n_buffer)
{
  if (network == AI_HANDLE_NULL) {
    network = (ai_handle)&AI_NET_OBJ_INSTANCE;
    AI_NETWORK_OBJ(network)->magic = AI_MAGIC_CONTEXT_TOKEN;
  }
  return ai_platform_outputs_get(network, n_buffer);
}


AI_API_ENTRY
ai_handle ai_network_destroy(ai_handle network)
{
  return ai_platform_network_destroy(network);
}


AI_API_ENTRY
ai_bool ai_network_init(
  ai_handle network, const ai_network_params* params)
{
  ai_network* net_ctx = AI_NETWORK_OBJ(ai_platform_network_init(network, params));
  ai_bool ok = true;

  if (!net_ctx) return false;
  ok &= network_configure_weights(net_ctx, params);
  ok &= network_configure_activations(net_ctx, params);

  ok &= ai_platform_network_post_init(network);

  return ok;
}


AI_API_ENTRY
ai_i32 ai_network_run(
  ai_handle network, const ai_buffer* input, ai_buffer* output)
{
  return ai_platform_network_process(network, input, output);
}


AI_API_ENTRY
ai_i32 ai_network_forward(ai_handle network, const ai_buffer* input)
{
  return ai_platform_network_process(network, input, NULL);
}



#undef AI_NETWORK_MODEL_SIGNATURE
#undef AI_NET_OBJ_INSTANCE
#undef AI_TOOLS_DATE_TIME
#undef AI_TOOLS_COMPILE_TIME

