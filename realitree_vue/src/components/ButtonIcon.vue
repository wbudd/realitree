<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <button
    :title="title"
    type="text"
  >
    <svg
      viewBox="0 0 24 24"
    >
      <slot/>
    </svg>
  </button>
</template>

<script>
export default {
  name: 'ButtonBase',
  props: {
    title: {
      type: String,
      required: true
    }
  }
}
</script>

<style lang="sass" scoped>

button
  width: 1.6rem
  height: 1.6rem
  padding: 0
  margin: 0
  border: none
  outline: none
  text-indent: 999%
  white-space: nowrap
  overflow: hidden
  cursor: pointer
  background-color: transparent

svg
  display: block

</style>
