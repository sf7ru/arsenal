// ***************************************************************************
// TITLE
//
// PROJECT
//
// ***************************************************************************
//
// FILE
//      $Id:$
// HISTORY
//      $Log:$
// ***************************************************************************

#include <arsenal.h>
#include <axhashing.h>

// ---------------------------------------------------------------------------
// ---------------------------------- DATA -----------------------------------
// -----|-------------------|-------------------------------------------------

const U64 m_crc64table[256] =
{
    0x0000000000000000LL, 0x24854997ba2f81e7LL, 0x490a932f745f03ceLL, 0x6d8fdab8ce708229LL,
    0x9215265ee8be079cLL, 0xb6906fc95291867bLL, 0xdb1fb5719ce10452LL, 0xff9afce626ce85b5LL,
    0x66daad56789639abLL, 0x425fe4c1c2b9b84cLL, 0x2fd03e790cc93a65LL, 0x0b5577eeb6e6bb82LL,
    0xf4cf8b0890283e37LL, 0xd04ac29f2a07bfd0LL, 0xbdc51827e4773df9LL, 0x994051b05e58bc1eLL,
    0xcdb55aacf12c7356LL, 0xe930133b4b03f2b1LL, 0x84bfc98385737098LL, 0xa03a80143f5cf17fLL,
    0x5fa07cf2199274caLL, 0x7b253565a3bdf52dLL, 0x16aaefdd6dcd7704LL, 0x322fa64ad7e2f6e3LL,
    0xab6ff7fa89ba4afdLL, 0x8feabe6d3395cb1aLL, 0xe26564d5fde54933LL, 0xc6e02d4247cac8d4LL,
    0x397ad1a461044d61LL, 0x1dff9833db2bcc86LL, 0x7070428b155b4eafLL, 0x54f50b1caf74cf48LL,
    0xd99a54b24bb2d03fLL, 0xfd1f1d25f19d51d8LL, 0x9090c79d3fedd3f1LL, 0xb4158e0a85c25216LL,
    0x4b8f72eca30cd7a3LL, 0x6f0a3b7b19235644LL, 0x0285e1c3d753d46dLL, 0x2600a8546d7c558aLL,
    0xbf40f9e43324e994LL, 0x9bc5b073890b6873LL, 0xf64a6acb477bea5aLL, 0xd2cf235cfd546bbdLL,
    0x2d55dfbadb9aee08LL, 0x09d0962d61b56fefLL, 0x645f4c95afc5edc6LL, 0x40da050215ea6c21LL,
    0x142f0e1eba9ea369LL, 0x30aa478900b1228eLL, 0x5d259d31cec1a0a7LL, 0x79a0d4a674ee2140LL,
    0x863a28405220a4f5LL, 0xa2bf61d7e80f2512LL, 0xcf30bb6f267fa73bLL, 0xebb5f2f89c5026dcLL,
    0x72f5a348c2089ac2LL, 0x5670eadf78271b25LL, 0x3bff3067b657990cLL, 0x1f7a79f00c7818ebLL,
    0xe0e085162ab69d5eLL, 0xc465cc8190991cb9LL, 0xa9ea16395ee99e90LL, 0x8d6f5faee4c61f77LL,
    0xf1c4488f3e8f96edLL, 0xd541011884a0170aLL, 0xb8cedba04ad09523LL, 0x9c4b9237f0ff14c4LL,
    0x63d16ed1d6319171LL, 0x475427466c1e1096LL, 0x2adbfdfea26e92bfLL, 0x0e5eb46918411358LL,
    0x971ee5d94619af46LL, 0xb39bac4efc362ea1LL, 0xde1476f63246ac88LL, 0xfa913f6188692d6fLL,
    0x050bc387aea7a8daLL, 0x218e8a101488293dLL, 0x4c0150a8daf8ab14LL, 0x6884193f60d72af3LL,
    0x3c711223cfa3e5bbLL, 0x18f45bb4758c645cLL, 0x757b810cbbfce675LL, 0x51fec89b01d36792LL,
    0xae64347d271de227LL, 0x8ae17dea9d3263c0LL, 0xe76ea7525342e1e9LL, 0xc3ebeec5e96d600eLL,
    0x5aabbf75b735dc10LL, 0x7e2ef6e20d1a5df7LL, 0x13a12c5ac36adfdeLL, 0x372465cd79455e39LL,
    0xc8be992b5f8bdb8cLL, 0xec3bd0bce5a45a6bLL, 0x81b40a042bd4d842LL, 0xa531439391fb59a5LL,
    0x285e1c3d753d46d2LL, 0x0cdb55aacf12c735LL, 0x61548f120162451cLL, 0x45d1c685bb4dc4fbLL,
    0xba4b3a639d83414eLL, 0x9ece73f427acc0a9LL, 0xf341a94ce9dc4280LL, 0xd7c4e0db53f3c367LL,
    0x4e84b16b0dab7f79LL, 0x6a01f8fcb784fe9eLL, 0x078e224479f47cb7LL, 0x230b6bd3c3dbfd50LL,
    0xdc919735e51578e5LL, 0xf814dea25f3af902LL, 0x959b041a914a7b2bLL, 0xb11e4d8d2b65faccLL,
    0xe5eb469184113584LL, 0xc16e0f063e3eb463LL, 0xace1d5bef04e364aLL, 0x88649c294a61b7adLL,
    0x77fe60cf6caf3218LL, 0x537b2958d680b3ffLL, 0x3ef4f3e018f031d6LL, 0x1a71ba77a2dfb031LL,
    0x8331ebc7fc870c2fLL, 0xa7b4a25046a88dc8LL, 0xca3b78e888d80fe1LL, 0xeebe317f32f78e06LL,
    0x1124cd9914390bb3LL, 0x35a1840eae168a54LL, 0x582e5eb66066087dLL, 0x7cab1721da49899aLL,
    0xa17870f5d4f51b49LL, 0x85fd39626eda9aaeLL, 0xe872e3daa0aa1887LL, 0xccf7aa4d1a859960LL,
    0x336d56ab3c4b1cd5LL, 0x17e81f3c86649d32LL, 0x7a67c58448141f1bLL, 0x5ee28c13f23b9efcLL,
    0xc7a2dda3ac6322e2LL, 0xe3279434164ca305LL, 0x8ea84e8cd83c212cLL, 0xaa2d071b6213a0cbLL,
    0x55b7fbfd44dd257eLL, 0x7132b26afef2a499LL, 0x1cbd68d2308226b0LL, 0x383821458aada757LL,
    0x6ccd2a5925d9681fLL, 0x484863ce9ff6e9f8LL, 0x25c7b97651866bd1LL, 0x0142f0e1eba9ea36LL,
    0xfed80c07cd676f83LL, 0xda5d45907748ee64LL, 0xb7d29f28b9386c4dLL, 0x9357d6bf0317edaaLL,
    0x0a17870f5d4f51b4LL, 0x2e92ce98e760d053LL, 0x431d14202910527aLL, 0x67985db7933fd39dLL,
    0x9802a151b5f15628LL, 0xbc87e8c60fded7cfLL, 0xd108327ec1ae55e6LL, 0xf58d7be97b81d401LL,
    0x78e224479f47cb76LL, 0x5c676dd025684a91LL, 0x31e8b768eb18c8b8LL, 0x156dfeff5137495fLL,
    0xeaf7021977f9cceaLL, 0xce724b8ecdd64d0dLL, 0xa3fd913603a6cf24LL, 0x8778d8a1b9894ec3LL,
    0x1e388911e7d1f2ddLL, 0x3abdc0865dfe733aLL, 0x57321a3e938ef113LL, 0x73b753a929a170f4LL,
    0x8c2daf4f0f6ff541LL, 0xa8a8e6d8b54074a6LL, 0xc5273c607b30f68fLL, 0xe1a275f7c11f7768LL,
    0xb5577eeb6e6bb820LL, 0x91d2377cd44439c7LL, 0xfc5dedc41a34bbeeLL, 0xd8d8a453a01b3a09LL,
    0x274258b586d5bfbcLL, 0x03c711223cfa3e5bLL, 0x6e48cb9af28abc72LL, 0x4acd820d48a53d95LL,
    0xd38dd3bd16fd818bLL, 0xf7089a2aacd2006cLL, 0x9a87409262a28245LL, 0xbe020905d88d03a2LL,
    0x4198f5e3fe438617LL, 0x651dbc74446c07f0LL, 0x089266cc8a1c85d9LL, 0x2c172f5b3033043eLL,
    0x50bc387aea7a8da4LL, 0x743971ed50550c43LL, 0x19b6ab559e258e6aLL, 0x3d33e2c2240a0f8dLL,
    0xc2a91e2402c48a38LL, 0xe62c57b3b8eb0bdfLL, 0x8ba38d0b769b89f6LL, 0xaf26c49cccb40811LL,
    0x3666952c92ecb40fLL, 0x12e3dcbb28c335e8LL, 0x7f6c0603e6b3b7c1LL, 0x5be94f945c9c3626LL,
    0xa473b3727a52b393LL, 0x80f6fae5c07d3274LL, 0xed79205d0e0db05dLL, 0xc9fc69cab42231baLL,
    0x9d0962d61b56fef2LL, 0xb98c2b41a1797f15LL, 0xd403f1f96f09fd3cLL, 0xf086b86ed5267cdbLL,
    0x0f1c4488f3e8f96eLL, 0x2b990d1f49c77889LL, 0x4616d7a787b7faa0LL, 0x62939e303d987b47LL,
    0xfbd3cf8063c0c759LL, 0xdf568617d9ef46beLL, 0xb2d95caf179fc497LL, 0x965c1538adb04570LL,
    0x69c6e9de8b7ec0c5LL, 0x4d43a04931514122LL, 0x20cc7af1ff21c30bLL, 0x04493366450e42ecLL,
    0x89266cc8a1c85d9bLL, 0xada3255f1be7dc7cLL, 0xc02cffe7d5975e55LL, 0xe4a9b6706fb8dfb2LL,
    0x1b334a9649765a07LL, 0x3fb60301f359dbe0LL, 0x5239d9b93d2959c9LL, 0x76bc902e8706d82eLL,
    0xeffcc19ed95e6430LL, 0xcb7988096371e5d7LL, 0xa6f652b1ad0167feLL, 0x82731b26172ee619LL,
    0x7de9e7c031e063acLL, 0x596cae578bcfe24bLL, 0x34e374ef45bf6062LL, 0x10663d78ff90e185LL,
    0x4493366450e42ecdLL, 0x60167ff3eacbaf2aLL, 0x0d99a54b24bb2d03LL, 0x291cecdc9e94ace4LL,
    0xd686103ab85a2951LL, 0xf20359ad0275a8b6LL, 0x9f8c8315cc052a9fLL, 0xbb09ca82762aab78LL,
    0x22499b3228721766LL, 0x06ccd2a5925d9681LL, 0x6b43081d5c2d14a8LL, 0x4fc6418ae602954fLL,
    0xb05cbd6cc0cc10faLL, 0x94d9f4fb7ae3911dLL, 0xf9562e43b4931334LL, 0xddd367d40ebc92d3LL
};

// ---------------------------------------------------------------------------
// -------------------------------- FUNCTIONS --------------------------------
// -----------------|---------------------------(|------------------|---------


// ***************************************************************************
// FUNCTION
//      crc64ecma_update
// PURPOSE
//
// PARAMETERS
//      U64   initial --
//      PVOID data    --
//      INT   size    --
// RESULT
//      U64 --
// ***************************************************************************
U64 crc64ecma_update(U64            initial,
                     PVOID          data,
                     INT            size)
{
    U64       qwCRC       = initial;
    PU8       pCurByte    = (PU8) data;
    PU8       pLastByte   = pCurByte + size;


    while (pCurByte != pLastByte)
    {
        qwCRC = (m_crc64table[((U8)qwCRC)^((U8)*pCurByte)]^((U64)qwCRC) >> 8);
        pCurByte++;
    }

    return qwCRC;
}
// ***************************************************************************
// FUNCTION
//
// PURPOSE
//
// PARAMETERS
//      PVOID data --
//      INT   size --
// RESULT
//      U64 crc64ecma_calc --
// ***************************************************************************
U64 crc64ecma_calc(PVOID         data,
                   INT           size)
{
    return crc64ecma_update(CRC64_INITIAL, data, size);
}

