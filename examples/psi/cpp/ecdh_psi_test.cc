// Copyright 2023 Ant Group Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "psi/cpp/ecdh_psi.h"

#include <algorithm>

#include "gtest/gtest.h"

#include "yacl/crypto/hash/hash_utils.h"

namespace yc = yacl::crypto;

namespace examples::psi {

namespace {
std::vector<std::string> CreateRangeItems(size_t begin, size_t size) {
  std::vector<std::string> ret;
  for (size_t i = 0; i < size; i++) {
    ret.push_back(std::to_string(begin + i));
  }
  return ret;
}

template <typename T>
inline std::vector<size_t> GetIntersectionIdx(const std::vector<T> &x,
                                              const std::vector<T> &y) {
  std::set<T> set(x.begin(), x.end());
  std::vector<size_t> ret;
  for (size_t i = 0; i < y.size(); ++i) {
    if (set.count(y[i]) != 0) {
      ret.push_back(i);
    }
  }
  return ret;
}
}  // namespace

TEST(PsiTest, Works) {
  size_t n = 4;
  auto x = CreateRangeItems(0, n);
  auto y = CreateRangeItems(3, n);

  EcdhPsi alice;
  EcdhPsi bob;

  // -------------------
  //       Step 1
  // -------------------
  std::vector<yc::EcPoint> x_points(n);
  // x_points = H(x) ^ {alice_sk}
  alice.MaskStrings(absl::MakeSpan(x), absl::MakeSpan(x_points));

  std::vector<yc::EcPoint> y_points(n);
  // y_points = H(y) ^ {bob_sk}
  bob.MaskStrings(absl::MakeSpan(y), absl::MakeSpan(y_points));

  // -------------------
  //       Step 2
  // -------------------
  //
  // Alice send x_points to bob, and bob send y_points to alice
  //
  // ... code here (omitted) ...
  //
  // You may mannually send the EcPoints through yacl::link::Context, which
  // handles an RPC channel, see: yacl/link/context.h. You may also use any
  // method that you like to let Alice talk to Bob. Remember the communication
  // channel needs to be a secure P2P channel.
  //
  // Since most of communication methods only accept strings or bytes, you may
  // serialize EcPoints by calling ec_->SerializePoint(/* ec points here */).
  // see: yacl/ecc/ecc_spi.h for more details.

  // -------------------
  //       Step 3
  // -------------------
  std::vector<uint128_t> y_final(n);
  // y_str = y_points ^ {alice_sk}
  alice.MaskEcPointsAndHashToU128(absl::MakeSpan(y_points),
                                  absl::MakeSpan(y_final));

  std::vector<uint128_t> x_final(n);
  // x_str = x_points ^ {bob_sk}
  bob.MaskEcPointsAndHashToU128(absl::MakeSpan(x_points),
                                absl::MakeSpan(x_final));

  /* check results */
  auto compare = GetIntersectionIdx(x, y);  // result
  auto z = GetIntersectionIdx(x_final, y_final);

  EXPECT_EQ(compare.size(), z.size());

  for (size_t i = 0; i < z.size(); ++i) {
    EXPECT_EQ(compare[i], z[i]);
  }
}

TEST(PsiExTest, Works) {
  size_t n = 4;
  auto x = CreateRangeItems(0, n);
  auto y = CreateRangeItems(3, n);

  EcdhPsi alice;
  EcdhPsi bob;

  // -------------------
  //       Step 1
  // -------------------
  std::vector<std::string> x_points = alice.MaskStringsEx(x);
  std::vector<std::string> y_points = bob.MaskStringsEx(y);

  // -------------------
  //       Step 2
  // -------------------
  //
  // Alice send x_points to bob, and bob send y_points to alice
  //
  // ... code here (omitted) ...
  //
  // You may mannually send the EcPoints through yacl::link::Context, which
  // handles an RPC channel, see: yacl/link/context.h. You may also use any
  // method that you like to let Alice talk to Bob. Remember the communication
  // channel needs to be a secure P2P channel.
  //
  // Since most of communication methods only accept strings or bytes, you may
  // serialize EcPoints by calling ec_->SerializePoint(/* ec points here */).
  // see: yacl/ecc/ecc_spi.h for more details.

  // -------------------
  //       Step 3
  // -------------------
  std::vector<uint128_t> y_final = alice.MaskEcPointsAndHashToU128Ex(y_points);
  std::vector<uint128_t> x_final = bob.MaskEcPointsAndHashToU128Ex(x_points);

  /* check results */
  auto compare = GetIntersectionIdx(x, y);  // result
  auto z = GetIntersectionIdx(x_final, y_final);

  EXPECT_EQ(compare.size(), z.size());

  for (size_t i = 0; i < z.size(); ++i) {
    EXPECT_EQ(compare[i], z[i]);
  }
}
}  // namespace examples::psi
