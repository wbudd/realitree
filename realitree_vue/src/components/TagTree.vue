<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <ul>
    <li
      v-for="[tag, childTagTree] in tagTree"
      :key="tag"
    >
      <svg
        v-if="hasParent"
        preserveAspectRatio="none"
        viewBox="0 0 10 10"
      >
        <polygon
          points="0,0 2,0 10,5 2,10 0,10 8,5"
          stroke-width="0"
        />
      </svg>
      <div>
        {{ tag }}
      </div>
      <tag-tree
        v-if="childTagTree.length"
        :has-parent="true"
        :tag-tree="childTagTree"
      />
    </li>
  </ul>
</template>

<script>
export default {
  name: 'TagTree',
  props: {
    hasParent: {
      type: Boolean,
      default: false
    },
    tagTree: {
      type: Array,
      required: true
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

ul
  display: flex
  flex-direction: column

li
  position: relative
  display: flex
  align-items: center
  border-top: 0.1rem solid $color-bg-c
  background-color: $color-bg-b

li:first-child
  border-top: none

li>ul
  flex-grow: 1

svg
  position: absolute
  width: 0.5rem
  height: 100%
  background-color: $color-bg-b

polygon
  fill: $color-bg-c

div
  margin: auto
  padding: 0.2rem

svg+div
  padding-left: 0.7rem

</style>
