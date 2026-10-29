/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    2026-07-04T23:27:17+0530
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[422] = {
  0x68c88194e3af4019U, 0xade3b18cfd362522U, 0x2ae9ae8f84a08781U, 0x1c8af00ddee4da28U,
  0x81c32357555569f4U, 0xe81e3e89dc3b0d03U, 0xc5bfbc818309d809U, 0x25592024d5f7bbe9U,
  0x2a28343c691a7f5aU, 0x6136274847bf23cbU, 0xdb1668454c187f13U, 0x2d39716d7f3b9b4aU,
  0x9c9fefbab99ed5caU, 0xf1811d19237b44d8U, 0xbe07c717a108fcb5U, 0xbf00000108U,
  0xfffffeb9ffffffefU, 0x94000000b8U, 0xffffff19000000cbU, 0x123aea30df0aeefdU,
  0xac801d7ebe401daU, 0x18db28d9e7ddf2dbU, 0xd35fb0be31cffefU, 0xef39e71d2eee1402U,
  0xfdc4abab0d81fb1fU, 0xddbf30471529f6f8U, 0xd2110ac4c3dbd403U, 0xb2a3efcce22d152aU,
  0x4440c13d35594040U, 0xd647fb00f3ac3727U, 0xf91dbae0de82cf1dU, 0xf6001b53f82ca632U,
  0xf27f5126336795e5U, 0x15ed09010eccdedeU, 0xeae0d9f207a2dc01U, 0xc7becdc4d38cf8c7U,
  0xc8dbf6c7cd8e04c4U, 0xfb2d19f20212c8d2U, 0xb296635b628ec41U, 0x7f7e4d5a3065ce04U,
  0x434090afff6e428U, 0xf04cdc011f5251eU, 0xe5f6fee2ec8130faU, 0xb31efde028bfebd7U,
  0xfa160e2dcef0d8d1U, 0x3093613f1fb06f7U, 0x415c0e18e640a5ebU, 0x2caa21f6f7530dafU,
  0x1a032a30ade6020aU, 0xd9afe24ff5bae3beU, 0xd259c2c73cf949ddU, 0x27f89781cc1e3a04U,
  0x2dd19fb2201e3747U, 0xd525d9cf09d21501U, 0x64e5f1a53d096f29U, 0xb0ec7e529784bfbU,
  0x17ed001a0e28edbfU, 0xc6815f2e04cffb91U, 0x1f172a31b1d09084U, 0xcf4d04452515d508U,
  0x10cdbffd2f4331ffU, 0xca86cda4b5195d66U, 0x91517e3f0f2bce5U, 0xf1f8e78224cef7e3U,
  0xcde4bc8c30f5e802U, 0xca24e32fad2831d8U, 0xcf0419d9c1c3d2abU, 0xcbf6ba81bec2a8e4U,
  0x81624cb32daf4eaU, 0x25da0a3138fbea43U, 0x564339362a7ff33aU, 0xcf04552f19370726U,
  0x26641c78f844143aU, 0x22493f05c2151f0cU, 0xf0051d23fcf1d5dfU, 0x5142b7f44b033c14U,
  0x29e2770e087fd4bbU, 0x39132f53443a9709U, 0x3f17ee1c4ef6e8cbU, 0xe224d4e8d2160df4U,
  0x31118fba1aef3007U, 0xc9a4aa1f0f4227edU, 0x6c62304d3d4a196cU, 0x703df10955430817U,
  0x5152df1e5b21cd5U, 0xe462bcdaebe8fbe4U, 0x3e420b22052afbb1U, 0xbb043e17fc3a1ba1U,
  0xee1c2729ad158183U, 0xfd92721c7b60ef1U, 0x5bc15b828e40914U, 0xcf22c302c03ebf3U,
  0xfcb33f11bee607d6U, 0x3b08404fe42d1ff5U, 0x1928da051a8103efU, 0xb9c9bfc8fa010dd6U,
  0x4548e9dc53a44a3fU, 0x22d3495c5bfd2602U, 0x40c375846e3cd97U, 0x30fd544ebc41f9c7U,
  0x6600ce8ed28e7d6U, 0x183e1a81cf991315U, 0xc00da0ba26d4cdafU, 0x493201e621c1fc20U,
  0xf7124846ed70edb4U, 0x13400b341e3bfc20U, 0x4a271b0b0440ea0fU, 0x553e022c4c40e02dU,
  0x2cd5d69d17da00e9U, 0xf58126c4fc3b22abU, 0xce24d5e4cf25f96bU, 0x285b293e3a60e91dU,
  0xae61bddf3810f27U, 0xe5c4cbfcf9e12f13U, 0x9ef050bdae112fdU, 0x472dfdfdfe3cd440U,
  0xf316e1acefeb1013U, 0x251e320ceae6f402U, 0xefde190827e7e510U, 0x30a0bd302f2faf8U,
  0xfd4acdc72625fd2bU, 0xe4adff34dedf2ceaU, 0xff93f2fb0fc81ccU, 0xd6fb1417cf06d508U,
  0x444c591615ee00U, 0x125a193d2f33ecd0U, 0xf6e208071ee4ef23U, 0xe0f7afb1f9f40ddaU,
  0x17f2c8a31cf303faU, 0xe0c706f905d7f9e2U, 0x81c4f41cad189e8bU, 0x55ffffffebU,
  0x6affffffcbU, 0x3a000000ebU, 0x7dffffffdbU, 0xfffffeeefffffefaU,
  0xffffff94ffffffe8U, 0x35fffffe6cU, 0xffffffa600000026U, 0x2010c3fde3eb10dcU,
  0x81b0ac09ace8b1d4U, 0xfe27051d06112cf0U, 0xf404eccbc2e836e3U, 0xecff21f00b3b06eaU,
  0xf240ff2d27313432U, 0x9f2e1338dce8bed4U, 0xceffc0ace3b0c481U, 0x2218ee35e1d435e3U,
  0xc03f01aea6d002d4U, 0xf0f942302e7d4e57U, 0xf728e01e410e4b19U, 0x4f5ee1caf565403aU,
  0x47967bf9fa060bdfU, 0xf75501dbe33ce66cU, 0x54dc7ff4f4d1e5e8U, 0xee6d1f57403c5f29U,
  0xb5d652035253f5adU, 0x198144dfe0c731dcU, 0x5c624d1235280647U, 0x22c90e25111bbecfU,
  0x240eb71cede5d6e0U, 0xe1b0bb239b83a5b2U, 0xc702cd2014ecbf1aU, 0xf4eb2823d2f9fbb2U,
  0x9a038106e9071a8eU, 0xd38f9f922ce17f9U, 0x1f26a1d5b84841c5U, 0x3edddc1e262cd4e0U,
  0x1329e3224f23dccfU, 0xa6dc9fada6b8e09dU, 0xc3fcc0faa325ab45U, 0xc2e918a8940a4440U,
  0xf53c358b0f3dfaf1U, 0xe72820ee81da2e35U, 0x9408f0179f493f3dU, 0xb6dbb3b83eeb212bU,
  0xa2e11ffcb583f41dU, 0x38deda96aee430c3U, 0x3ee9c3fc1a1e0330U, 0x81353407820b6618U,
  0x8504accb0318fb3dU, 0x25815d2a47d9b8cdU, 0x5b6a110042e71843U, 0xb4d33ddd0fe4ae9dU,
  0xee1c93cbb350e16U, 0x38ab91819c7c9ecU, 0xfb37cfcdd6b032d3U, 0xa73bb43bbf1ea0c3U,
  0x1745a68fadd1d19aU, 0xe75ff82eeeef1fe6U, 0xa1ece68e1605e2edU, 0x5ac90045ff4e722dU,
  0x427ffad412e80e02U, 0x36d11b2af518c6ddU, 0x6c38f04dd1250b55U, 0xf6b763d8f1ed9edaU,
  0x2e25f8b6da3eb60aU, 0xeb9e8547faf5c681U, 0xefcb910117a4b0b4U, 0xc110a92c08c42406U,
  0x81c3e0cdc2d6bc21U, 0xeb031b41343f2623U, 0xe5c9f2ef3b2c26d6U, 0x31d9e73223e7ce17U,
  0x3b0cfb430f3bcde3U, 0xa5131a3fd456e444U, 0xe9b051fa2621bae2U, 0x4369344f2ede4053U,
  0xb48c5230272e52adU, 0xccbda24ad6bbbe3dU, 0x41c97fecb735f9c9U, 0xd96f18f74b9dc3e1U,
  0x482a42ad25f6e612U, 0xa72e81ae30f83937U, 0x1fdbc1eeb0b8e8d0U, 0xccf72fc6130016dfU,
  0xed00c83b35380a29U, 0x1c40acf0bef90dafU, 0x81dbdd0cd1ba98dcU, 0xfaf219dcee210539U,
  0xcee11c00c9d0371eU, 0xf91711e50b4f04dbU, 0xc4b084c37f5e641U, 0xdd4298b3188967b8U,
  0x8662e33daff78621U, 0xc2b2f139205119fU, 0x45d83344bfe707bbU, 0xa6293229d5e66dddU,
  0xa784a24e81e9d2d8U, 0xc2815f5e604bb7abU, 0x3f4b04c1d03f4806U, 0x8ce0be125d91fbbU,
  0x10519612bbc90de5U, 0xb46200e82a2becaaU, 0x951b0aa9e13fdcaU, 0x48a0ef1c31c9cad8U,
  0x3a1ccb3a0622505bU, 0x17c1da11fdbfe0e0U, 0x433cd80be4e3fe08U, 0xccf61bd5c181b9caU,
  0xae1bf3f0ceb40c25U, 0x472cb5b20b30d96dU, 0x33951f3656e3cc37U, 0xb951d837e8315d22U,
  0x16e700cfd8d1d8ddU, 0x3718b9e052f952f7U, 0xe8097fd13906fcd0U, 0xf724d7ebb9dc07f1U,
  0x81d68896efdb8bc0U, 0x812c5faf013ea1eU, 0xbf09d8a1ded91da2U, 0x11163bf84f4c0356U,
  0x43bf247395419ecU, 0x1de54c16da22b7dbU, 0xc50581ee0a22f4c5U, 0xe9fdd0eabd25fefcU,
  0xf498c1510db03b9U, 0x2f5ef40443dc80bU, 0xf026d10c12163902U, 0x2f63f659c2f200e5U,
  0x56ca03c843f10b1dU, 0x1adee9514b543c14U, 0xe38d3d55bedd1059U, 0x60fdb8bfa632ed66U,
  0xedb67f2fb2252c29U, 0x3c844406e0e9e6ceU, 0x3d523963f4ea3d45U, 0xf5fc2ce413f5dcadU,
  0x4026deea2d110415U, 0xc2d3e1fac085c381U, 0x19e0fb06e2fcb5f5U, 0x9b2455501ccae1d8U,
  0x42d79c2945bdc6c7U, 0x43f00136e16a3e16U, 0x193a81cd1b0f37b5U, 0xaf3bf4e230e642e5U,
  0xf81d35be1d2f4c1eU, 0xc130a560b9d8d037U, 0x34dbee1d27d13b01U, 0xe1df0f2b6fdddfdU,
  0x24e61de0efac0611U, 0xd37f3fc6fde2ebccU, 0xdcc8e5dd5110e2ecU, 0x369ed28bbf43c470U,
  0x142b8192bf2313a9U, 0xd8aef2a94aafe426U, 0xc9869817e60327c7U, 0x13455aaf114db5cU,
  0x3152429bee10ce4aU, 0xe3e32bc0f5fcc3a1U, 0xaa2781062ae1c8f9U, 0x1b15210d361227c5U,
  0xe4e79ac00117081eU, 0x2e0c5af1393a06e2U, 0xda27c9fb25eabdf0U, 0xc11d1043f40c8104U,
  0xc7e2fcdece19facaU, 0xf009e5e205fff0ebU, 0xd1c612048c01ea88U, 0xf4fc2a112d572a07U,
  0xcf1a2ce50fef233cU, 0xabff5a11ad009caU, 0x4c41262e3ed22235U, 0x4b12ac8e641eebdU,
  0xffefd0e6203bd0dcU, 0xfea18132acc3a0feU, 0xfe97f3e2f2efb625U, 0x2e5a403fd6213a48U,
  0x5b8367e30af04208U, 0xfd3f9d49f927ff4aU, 0x2cb9eb01bd40480fU, 0x517f22e35e5418d7U,
  0x26fa5540b3c6cf36U, 0xb742ab22b717b4d3U, 0xe8b3b59b8bd391d4U, 0xd134eafac823e926U,
  0x9a35e19fb61c1e07U, 0x57261c22772f4728U, 0xe97f070d01f11909U, 0xe30dd24bd6dec198U,
  0xdf0a8198eb0fc411U, 0xdc5808e301f549f9U, 0xc20c05d2b8d1e1a6U, 0x3529333a0a41f452U,
  0xf240de382e5134f1U, 0x49fd0e2fa8398413U, 0x277985112c4531e2U, 0x5013e00f1256a934U,
  0xff76dd11f65ad024U, 0xa22dd9aa34061381U, 0x40e7c94151001943U, 0x28ffffffffU,
  0xffffffdd00000096U, 0xffffffbe00000003U, 0x22ffffffcdU, 0x38fffffffbU,
  0x5d00000053U, 0xffffffecffffffa1U, 0x48ffffffe7U, 0x57fffffffdU,
  0x3a0000003aU, 0x5000000e8U, 0xffffffcaffffffdfU, 0x130000015cU,
  0x240000001bU, 0x2d00000082U, 0xfffffff10000002cU, 0x607372f4baf1a69U,
  0x18116bf4d01d1c59U, 0x9eee45ca424fc71dU, 0x296c78257f14ec1aU, 0xe5f122dffa3d81b7U,
  0xd9dfc9cc1eb19fe0U, 0x58cb8bfdd4b52686U, 0x148e8d39df87001dU, 0xdc1e3600f512a9faU,
  0xb2b2d52a9331c505U, 0x1f1f0e2dbc1e81f2U, 0xeeac0b0800c2ce8fU, 0xf8e83ad4b30a223eU,
  0x260b9eca458b6fbaU, 0x82f8a4a2e6cfdc25U, 0x38d4d1d93b81fee2U, 0x31264a1bc2112623U,
  0x81df3248ae2e9016U, 0x2fe421d4da0927fcU, 0x9c8101ec1bfff9c8U, 0xadfbe87fcf0f735U,
  0x810bddda9bdd9839U, 0xa4b6340aa1d050d7U, 0xf023aafae0d8919dU, 0x59a89411a09413b8U,
  0xc81db48401423d5U, 0x15f4c91ab23b85f8U, 0xce1dae232dff9bbeU, 0xff09ef2c13f33027U,
  0x21e612010be91526U, 0xd1e317062934e4fcU, 0xdef41bf37f3bf919U, 0xa8d42c009b099e81U,
  0xca3aaeefe7db0baeU, 0x2c1ab72fbaa13ee1U, 0xc58bae20ffd4ed37U, 0x5f1c443e2fce226eU,
  0x38d05fbbbf0b2f25U, 0x81e434ff5c411359U, 0x2c364bc5343340deU, 0xb52b31012bebf751U,
  0x815e86ee534b85f7U, 0x179ff21b03fe9a92U, 0xb0af2a123ebecff4U, 0x3ed0dfc768eb7f32U,
  0xf0be01fec2e73f5eU, 0x833477f84247d11fU, 0xc5e46b640e92823U, 0xd1e8e7653a0ae8cbU,
  0xb5d52dbb42fd5019U, 0x239d26d7bf8d8184U, 0x2fe4ec1be0de2982U, 0xb3062bcffa4096d9U,
  0xdd37f30ceaf2b381U, 0x5019bc26e6d3e0fdU, 0xf38fda0507b39502U, 0xf5ed79fa0aa432b9U,
  0xf07dcd9f7dbf20e6U, 0x751ab33921fb81e6U, 0xb9b60e817276ed7aU, 0x17d423377fd95b12U,
  0x271941b81bcdf515U, 0xc9123dfbe448e74eU, 0xe2d54cc594d20f2U, 0x1e0000000ddU,
  0xfffffed7fffffe82U, 0xfffffd9bfffffea7U, 0xffffffbd00000000U, 0x200000304U,
  0xffffff74fffffea1U, 0x217ffffff42U, 0xffffffff00000000U, 0x5343d8c5b5019f41U,
  0x5bd5a0b36f353781U, 0xffffffa4U,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

