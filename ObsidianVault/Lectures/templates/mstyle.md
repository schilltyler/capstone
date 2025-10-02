<style>
/* Full slide canvas */
.reveal section {
  width: 100%;
  height: 100%;
  overflow: hidden;
  position: relative;
}

/* Mermaid block inside slides */
.reveal section .mermaid {
  margin: 0 !important;
  padding: 0 !important;
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
  overflow: hidden;
}

/* Force SVG scaling */
.reveal section .mermaid > svg {
  width: 100% !important;
  height: 100% !important;
  max-width: none !important;
  max-height: none !important;
  display: block;
}

/* Increase text size inside diagrams */
.reveal section .mermaid svg text {
  font-size: 32px !important;
}




.reveal .mermaid svg {
  min-width: 100%;
  height: auto;
}

.reveal .mermaid svg .label-container {
  fill: var(--r-background-color) !important;
}

.reveal .mermaid svg .label foreignObject {
  overflow: visible !important;
}

.reveal .mermaid svg .nodeLabel {
  color: var(--r-main-color) !important;
}

.reveal .mermaid svg .edgePaths path {
  stroke: var(--r-main-color) !important;
}

.reveal .mermaid svg marker {
  stroke: var(--r-main-color) !important;
  fill: var(--r-main-color) !important;
}

.reveal .mermaid svg .edgeLabel {
  background-color: var(--r-background-color) !important;
  color: var(--r-main-color) !important;
  font-size: 13px;
}


</style>