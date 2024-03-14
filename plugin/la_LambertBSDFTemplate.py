import maya.mel
from mtoa.ui.ae.shaderTemplate import ShaderAETemplate

class AEla_LambertBSDFTemplate(ShaderAETemplate):
    def setup(self):
        self.addSwatch()
        self.beginScrollLayout()
        self.addCustom('message', 'AEshaderTypeNew', 'AEshaderTypeReplace')

        self.beginLayout('Color map', collapse=False)
        self.addControl("albedoSample", label = "Albedo Sample")
        self.addControl('albedo', label='Albedo')
        self.endLayout()

        self.beginLayout('Tree stem properties', collapse=False)
        self.addControl("stemSample", label = "Stem Sample")
        self.addControl("Scale", label="Scale")
        self.addControl("Offset", label="Offset")
        self.addControl("Orientation", label="Orientation")
        self.endLayout()

        self.beginLayout('Knot properties', collapse=False)
        self.addControl("knotSample", label = "Knot Sample")
        self.addControl("Density", label="Density")
        self.addControl("Lifespan", label="Lifespan")
        self.endLayout()

        maya.mel.eval('AEdependNodeTemplate '+self.nodeName)
        self.addExtraControls()
        self.endScrollLayout()
