; RUN: opt -load-pass-plugin %llvmshlibdir/example_LLVM_IR%pluginext\
; RUN: -passes=example -S %s | FileCheck %s

define i32 @_Z7mul_rhsi(i32 noundef %a) {
; CHECK-LABEL: @_Z7mul_rhsi
; CHECK-NEXT:  %1 = shl i32 %a, 3
; CHECK-NEXT:  ret i32 %1
  %mul = mul nsw i32 %a, 8
  ret i32 %mul
}

define i32 @_Z7mul_lhsi(i32 noundef %a) {
; CHECK-LABEL: @_Z7mul_lhsi
; CHECK-NEXT:  %1 = shl i32 %a, 4
; CHECK-NEXT:  ret i32 %1
  %mul = mul nsw i32 16, %a
  ret i32 %mul
}

define i32 @_Z9udiv_testj(i32 noundef %a) {
; CHECK-LABEL: @_Z9udiv_testj
; CHECK-NEXT:  %1 = lshr i32 %a, 2
; CHECK-NEXT:  ret i32 %1
  %div = udiv i32 %a, 4
  ret i32 %div
}

define i32 @_Z9sdiv_testi(i32 noundef %a) {
; CHECK-LABEL: @_Z9sdiv_testi
; CHECK-NEXT:  %1 = icmp slt i32 %a, 0
; CHECK-NEXT:  %2 = add i32 %a, 31
; CHECK-NEXT:  %3 = select i1 %1, i32 %2, i32 %a
; CHECK-NEXT:  %4 = ashr i32 %3, 5
; CHECK-NEXT:  ret i32 %4
  %div = sdiv i32 %a, 32
  ret i32 %div
}

define i32 @_Z12mul_not_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z12mul_not_pow2i
; CHECK-NEXT:  %mul = mul nsw i32 %a, 7
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, 7
  ret i32 %mul
}

define i32 @_Z13sdiv_not_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z13sdiv_not_pow2i
; CHECK-NEXT:  %div = sdiv i32 %a, 10
; CHECK-NEXT:  ret i32 %div
  %div = sdiv i32 %a, 10
  ret i32 %div
}

define i32 @_Z17mul_negative_pow2i(i32 noundef %a) {
; CHECK-LABEL: @_Z17mul_negative_pow2i
; CHECK-NEXT:  %mul = mul nsw i32 %a, -8
; CHECK-NEXT:  ret i32 %mul
  %mul = mul nsw i32 %a, -8
  ret i32 %mul
}
