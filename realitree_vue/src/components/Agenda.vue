<!-- SPDX-License-Identifier: MIT -->
<!-- Copyright © 2019 William Budd -->

<template>
  <div
    id="agenda"
  >
    <div
      id="tabs"
    >
      <button-add
        v-if="tabIndex === 1"
        title="Add a new project root"
        @click.native="addRootProject"
      />
      <div
        id="tasksTab"
        :class="{ currentTab: tabIndex === 0 }"
        @click="tabIndex = 0"
      >
        Tasks
      </div>
      <div
        id="projectsTab"
        :class="{ currentTab: tabIndex === 1 }"
        @click="tabIndex = 1"
      >
        Projects
      </div>
      <div
        id="historyTab"
        :class="{ currentTab: tabIndex === 2 }"
        @click="tabIndex = 2"
      >
        History
      </div>
    </div>
    <keep-alive>
      <task-list
        v-if="tabIndex === 0"
        id="content"
        :tasks="tasks"
      />
      <project-list
        v-else-if="tabIndex === 1"
        id="content"
        :depth-is-odd="true"
        :parent-project="rootProject"
      />
      <div
        v-else
        id="content"
      >
        todo
      </div>
    </keep-alive>
    <task-new
      v-show="tabIndex < 2"
      :projects="rootProject.children"
      :tasks="tasks"
    />
  </div>
</template>

<script>
import ButtonAdd from './ButtonAdd'
import ProjectList from './ProjectList'
import TaskList from './TaskList'
import TaskNew from './TaskNew'

export default {
  name: 'Agenda',
  components: {
    ButtonAdd,
    ProjectList,
    TaskList,
    TaskNew
  },
  inject: {
    ws: { from: 'realitreeWebSocket' }
  },
  data () {
    return {
      rootProject: {
        id: 0,
        children: []
      },
      tabIndex: 0,
      tasks: []
    }
  },
  created () {
    this.ws.updateProjects = v => { this.rootProject.children = v }
  },
  methods: {
    addRootProject () {
      const newId = this.ws.newId++
      this.ws.sendProjectAdd(0, newId)
      this.rootProject.children.push({
        id: newId,
        title: '',
        description: '',
        isCollapsed: false,
        isDraggable: false,
        isTagged: false,
        children: []
      })
      this.$nextTick(() => {
        let elem = document.getElementById('content')
        elem.scrollTop = elem.scrollHeight - elem.clientHeight
      })
    }
  }
}
</script>

<style lang="sass" scoped>
@import "../conf.sass"

#agenda
  position: fixed
  left: 0
  right: 0
  top: 0
  bottom: 0
  display: flex
  flex-direction: column

#tabs
  position: relative
  display: flex
  justify-content: center
  border-bottom: 0.2rem solid $color-bg-a

#tabs>button
  position: absolute
  left: 0
  bottom: 0
  background-color: $color-bg-a

#tabs>div
  padding: 1rem 5rem
  border-left: 0.1rem solid $color-bg-a
  border-right: 0.1rem solid $color-bg-a
  cursor: pointer

#tabs>#tasksTab
  border-left: 0.2rem solid $color-bg-a

#tabs>#historyTab
  border-right: 0.2rem solid $color-bg-a

#tabs>div:hover
  background-color: $color-bg-b

#tabs>div.currentTab
  cursor: auto
  background-color: $color-bg-a

#content
  flex: 1
  z-index: -1
  overflow: auto
  background-color: $color-bg-a

</style>
