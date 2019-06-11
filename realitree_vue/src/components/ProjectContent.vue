<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <div
    v-click-outside="clickOutsideProject"
    :class="{
      bgColorMain: !depthIsOdd,
      bgColorAlt: depthIsOdd,
      dragHandle: project.isDraggable
    }"
  >
    <button-collapse-expand
      :value="project.isCollapsed"
      title-collapse="Collapse this project node"
      title-expand="Expand this project node"
      @input="v => toggleCollapsed(v)"
    />
    <h3
      v-click-outside="clickOutsideTitle"
      ref="title"
      contenteditable="plaintext-only"
      placeholder="Title"
      v-text="project.title"
    />
    <p
      v-click-outside="clickOutsideDescription"
      v-if="showProjectDescription && !project.isCollapsed"
      ref="description"
      contenteditable="plaintext-only"
      placeholder="Description"
      v-text="project.description"
    />
    <div
      v-if="!project.isCollapsed"
    >
      <button-add
        :class="{ bgColorMain: depthIsOdd, bgColorAlt: !depthIsOdd }"
        title="Add a new project"
        @click.native="addChildProject"
      />
      <button-delete
        v-if="!project.children.length"
        title="Abort this project"
        @click.native="abortProject"
      />
      <button-tag
        v-if="!project.children.length"
        v-model="project.isTagged"
        title-tag="Tag a new task with this project"
        title-untag="Untag this project from the new task"
      />
      <button-description
        v-model="showProjectDescription"
        title-show="Show description"
        title-hide="Hide description"
      />
      <button-drag
        title="Activate drag mode"
        @click.native="project.isDraggable = true"
      />
    </div>
  </div>
</template>

<script>
import ButtonAdd from './ButtonAdd'
import ButtonCollapseExpand from './ButtonCollapseExpand'
import ButtonDelete from './ButtonDelete'
import ButtonDescription from './ButtonDescription'
import ButtonDrag from './ButtonDrag'
import ButtonTag from './ButtonTag'

export default {
  name: 'ProjectContent',
  components: {
    ButtonAdd,
    ButtonCollapseExpand,
    ButtonDelete,
    ButtonDescription,
    ButtonDrag,
    ButtonTag
  },
  inject: {
    ws: { from: 'realitreeWebSocket' }
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
  data () {
    return {
      showProjectDescription: this.project.description.length > 0
    }
  },
  methods: {
    addChildProject () {
      const newId = this.ws.newId++
      this.ws.sendProjectAdd(this.project.id, newId)
      this.project.children.push({
        id: newId,
        title: '',
        description: '',
        isCollapsed: false,
        isDraggable: false,
        isTagged: false,
        children: []
      })
    },
    abortProject () {
      const i = this.parentProject.children.indexOf(this.project)
      this.ws.sendProjectAbort(this.parentProject.id, i)
      this.parentProject.children.splice(i, 1)
    },
    toggleCollapsed (v) {
      this.ws.sendProjectCollapsed(this.project.id, v)
      this.project.isCollapsed = v
    },
    clickOutsideProject () {
      this.project.isDraggable = false
    },
    clickOutsideTitle () {
      const title = this.$refs.title.innerText
      if (title !== this.project.title &&
          this.parentProject.children.includes(this.project)) {
        // to do: use (existing) code to continuously send string diff instead
        this.ws.sendProjectTitle(this.project.id, title)
        this.project.title = title
      }
    },
    clickOutsideDescription () {
      const description = this.$refs.description.innerText
      if (description !== this.project.description &&
          this.parentProject.children.includes(this.project)) {
        // to do: use (existing) code to continuously send string diff instead
        this.ws.sendProjectDescription(this.project.id, description)
        this.project.description = description
      }
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

li>div
  position: relative
  display: flex
  flex-direction: column
  min-width: 3.2rem

li>div>button
  position: absolute
  right: 0
  top: 0

h3, p
  margin: 0.3rem 0.5rem
  outline: none

h3::before
  content: ''
  float: right
  width: 1.1rem
  height: 1.3rem

h3[placeholder]:empty:after,
p[placeholder]:empty:after
  content: attr(placeholder)
  color: $color-fg-b

div>div
  display: flex
  flex-flow: row-reverse wrap-reverse
  margin-top: auto

.dragHandle::after
  content: ''
  position: absolute
  left: 0
  right: 0
  top: 0
  bottom: 0
  z-index: 1
  background-color: rgba(0, 255, 0, 0.3)

#content.projectDrag .dragHandle
  max-width: 3.2rem

</style>
