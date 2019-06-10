<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <draggable
    :options="dragOptions"
    :value="parentProject.children"
    element="ul"
    @input="v => setProjects(v)"
    @start="onDragStart($event)"
    @end="onDragEnd($event)"
  >
    <project-container
      v-for="project in parentProject.children"
      :key="project.id"
      :depth-is-odd="!depthIsOdd"
      :parent-project="parentProject"
      :project="project"
      class="draggableItem"
    />
  </draggable>
</template>

<script>
import draggable from 'vuedraggable'

export default {
  name: 'ProjectList',
  components: {
    draggable,
    ProjectContainer: () => import('./ProjectContainer')
  },
  inject: {
    ws: { from: 'kaizenWebSocket' }
  },
  props: {
    depthIsOdd: {
      type: Boolean,
      required: true
    },
    parentProject: {
      type: Object,
      required: true
    }
  },
  computed: {
    dragOptions () {
      return {
        draggable: '.draggableItem',
        group: 'draggableProjects',
        handle: '.dragHandle'
      }
    }
  },
  methods: {
    setProjects (newProjects) {
      // vuedraggable's (or Sortable.js') @end event seems broken, so all
      // finalization/processing is handled here instead
      const oldProjects = this.parentProject.children
      if (Math.abs(newProjects.length - oldProjects.length) > 1) {
        console.error(
          'setProjects() received projects with a length diff greater than 1'
        )
        return
      }
      let i = 0
      if (newProjects.length < oldProjects.length) {
        for (let newProject of newProjects) {
          if (newProject.id !== oldProjects[i].id) {
            break
          }
          i++
        }
        const target = this.ws.draggedProjectTarget
        this.ws.sendProjectMove(this.parentProject.id, target.parentId,
                                i, target.index)
      } else if (newProjects.length > oldProjects.length) {
        for (let oldProject of oldProjects) {
          if (oldProject.id !== newProjects[i].id) {
            break
          }
          i++
        }
        this.ws.draggedProjectTarget.parentId = this.parentProject.id
        this.ws.draggedProjectTarget.index = i
        newProjects[i].isDraggable = false
      } else {
        while (oldProjects[i].id === newProjects[i].id) {
          i++
        }
        let j = oldProjects.length - 1
        while (oldProjects[j].id === newProjects[j].id) {
          j--
        }
        if (oldProjects[i].id === newProjects[j].id) {
          this.ws.sendProjectMove(this.parentProject.id, this.parentProject.id,
                                  i, j)
        } else {
          this.ws.sendProjectMove(this.parentProject.id, this.parentProject.id,
                                  j, i)
        }
      }
      this.parentProject.children = newProjects
    },
    onDragStart () {
      // console.log('onDragStart',
      //            document.getElementById('content').classList)
      document.getElementById('content').classList = ['projectDrag']
    },
    onDragEnd () {
      // console.log('onDragEnd',
      //            document.getElementById('content').classList)
      document.getElementById('content').classList = []
      this.parentProject.children.forEach(p => { p.isDraggable = false })
    }
  }
}
</script>

<style lang="sass" scoped>

ul
  margin: -0.2rem 0

#content.projectDrag ul
  min-width: 3.2rem
  min-height: 3.2rem

</style>
