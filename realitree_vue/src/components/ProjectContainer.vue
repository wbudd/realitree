<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <li>
    <Project-content
      :depth-is-odd="depthIsOdd"
      :parent-project="parentProject"
      :project="project"
      :style="{ maxWidth: widthCraft.content }"
      @toggleDraggable="v => isDraggable = v"
    />
    <project-list
      v-if="!project.isCollapsed && !project.isDraggable"
      :depth-is-odd="depthIsOdd"
      :parent-project="project"
      :style="{ maxWidth: widthCraft.children }"
    />
  </li>
</template>

<script>
import ProjectContent from './ProjectContent'
import ProjectList from './ProjectList'

export default {
  name: 'ProjectContainer',
  components: {
    ProjectContent,
    ProjectList
  },
  props: {
    depthIsOdd: {
      type: Boolean,
      required: true
    },
    parentProject: {
      type: Object,
      required: true
    },
    project: {
      type: Object,
      required: true
    }
  },
  computed: {
    widthCraft () {
      const getTreeGrowth = project => project.isCollapsed ? [0, []] : project
        .children.reduce(
          ([totalLeafCount, longestLeafCountArray], childProject) => {
          let [leafCount, leafCountArray] = getTreeGrowth(childProject)
          leafCountArray.push(leafCount)
          const sum = a => a.reduce((prev, cur) => prev + cur, 0)
          return [
            leafCount ? totalLeafCount + leafCount : totalLeafCount + 1,
            leafCountArray.length === longestLeafCountArray.length
            ? (sum(leafCountArray) < sum(longestLeafCountArray)
               ? leafCountArray : longestLeafCountArray)
            : (leafCountArray.length > longestLeafCountArray.length
               ? leafCountArray : longestLeafCountArray)
          ]
        }, [0, []])
      const [leafCount, leafCounts] = getTreeGrowth(this.project)
      const percentage = 100 / (1 + leafCount + leafCounts.slice(1)
        .reduce((size, count) => size + leafCount / count, 0))
      return {
        content: percentage + '%',
        children: 100 - percentage + '%'
      }
    }
  }
}
</script>

<style lang="sass" scoped>

li
  display: flex
  background-color: transparent
  margin: 0.2rem 0

</style>
