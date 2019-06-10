<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <div>
    <table>
      <tr>
        <th>
          Earliest
        </th>
        <td>
          Now
        </td>
      </tr>
      <tr>
        <th>
          Latest
        </th>
        <td>
          Never
        </td>
      </tr>
    </table>
    <input
      type="text"
      placeholder="New task description"
      v-model="description"
    />
    <tag-tree
      class="tagTree"
      :tag-tree="tagTree"
    />
  </div>
</template>

<script>
import TagTree from './TagTree'

export default {
  name: 'TaskNew',
  components: {
    TagTree
  },
  inject: {
    ws: { from: 'kaizenWebSocket' }
  },
  props: {
    projects: {
      type: Array,
      required: true
    },
    tasks: {
      type: Array,
      required: true
    }
  },
  data () {
    return {
      description: ''
    }
  },
  computed: {
    tagTree () {
      const getTagTree = projects => projects.reduce((tagTree, project) => {
        if (project.isTagged) {
          tagTree.push([project.title, []])
        } else {
          const tagBranch = getTagTree(project.children)
          if (tagBranch.length) {
            tagTree.push([project.title, tagBranch])
          }
        }
        return tagTree
      }, [])
      return getTagTree(this.projects)
    }
  },
  methods: {
    addTask () {
      const newId = this.ws.newId++
      this.tasks.splice(0, 0, {
        id: newId,
        status: 0, // to do: make this an enum or some shit
        description: '',
        tags: [],
        earliest: 0,
        latest: 0,
        switchTimes: [],
        afterword: ''
      })
      this.isCreatingNewTask = false // WTF was this supposed to mean?
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

div
  display: flex
  flex-wrap: wrap
  padding: 0.2rem
  border-top: 0.2rem solid $color-bg-a
  background-color: $color-bg-c

table
  border-spacing: 0.3rem

th
  text-align: right

td
  text-decoration: underline
  color: $color-date
  cursor: pointer

input
  flex: 1
  min-width: 50rem
  margin: 0 0.2rem
  padding: 0.2rem 0.3rem
  border: none
  outline: none
  color: $color-fg-a
  background-color: $color-bg-b

input::placeholder
  color: $color-fg-b

</style>
