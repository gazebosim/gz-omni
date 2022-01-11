###############################################################################
#
# Copyright 2020 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################

import omni.client
from pxr import Gf, Kind, Sdf, Usd, UsdLux, UsdGeom, UsdShade
import math
import argparse

connectionStatusSubscription = None
stage = None

def logCallback(threadName, component, level, message):
    if loggingEnabled:
        print(message)


def connectionStatusCallback(url, connectionStatus):
    if connectionStatus is omni.client.ConnectionStatus.CONNECT_ERROR:
        sys.exit("[ERROR] Failed connection, exiting.")


def startOmniverse(doLiveEdit):
    omni.client.set_log_callback(logCallback)
    if loggingEnabled:
        omni.client.set_log_level(omni.client.LogLevel.DEBUG)

    if not omni.client.initialize():
        sys.exit("[ERROR] Unable to initialize Omniverse client, exiting.")

    connectionStatusSubscription = omni.client.register_connection_status_callback(connectionStatusCallback)

    omni.client.usd_live_set_default_enabled(doLiveEdit)


def shutdownOmniverse():
    omni.client.usd_live_wait_for_pending_updates()

    connectionStatusSubscription = None

    omni.client.shutdown()


def isValidOmniUrl(url):
    omniURL = omni.client.break_url(url)
    if omniURL.scheme == "omniverse" or omniURL.scheme == "omni":
        return True
    return False


def createOmniverseModel(path):
    print("Creating Omniverse stage")
    global context, stage

    stageUrl = path + "/helloworld_py.usd"
    omni.client.delete(stageUrl)

    stage = Usd.Stage.CreateNew(stageUrl)
    UsdGeom.SetStageUpAxis(stage, UsdGeom.Tokens.y)
    UsdGeom.SetStageMetersPerUnit(stage, 0.01)

    print("Created stage:", stageUrl)

    return stageUrl

# This function will add a commented checkpoint to a file on Nucleus if:
#   Live mode is disabled (live checkpoints are ill-supported)
#   The Nucleus server supports checkpoints
def checkpointFile(stageUrl, comment):
    if omni.client.usd_live_get_default_enabled():
        return

    result, serverInfo = omni.client.get_server_info(stageUrl)
    
    if result and serverInfo and serverInfo.checkpoints_enabled:
        bForceCheckpoint = True
        print("Adding checkpoint comment <%s> to stage <%s>" % (comment, stageUrl))
        omni.client.create_checkpoint(stageUrl, comment, bForceCheckpoint)


def printConnectedUsername(stageUrl):
    result, serverInfo = omni.client.get_server_info(stageUrl)

    if serverInfo:
        print("Connected username:", serverInfo.username)

h = 50.0
boxVertexIndices = [ 0,  1,  2,  1,  3,  2,
                     4,  5,  6,  4,  6,  7,
                     8,  9, 10,  8, 10, 11,
                    12, 13, 14, 12, 14, 15,
                    16, 17, 18, 16, 18, 19,
                    20, 21, 22, 20, 22, 23 ]
boxVertexCounts = [ 3 ] * 12
boxNormals = [ ( 0,  0, -1), ( 0,  0, -1), ( 0,  0, -1), ( 0,  0, -1),
               ( 0,  0,  1), ( 0,  0,  1), ( 0,  0,  1), ( 0,  0,  1),
               ( 0, -1,  0), ( 0, -1,  0), ( 0, -1,  0), ( 0, -1,  0),
               ( 1,  0,  0), ( 1,  0,  0), ( 1,  0,  0), ( 1,  0,  0),
               ( 0,  1,  0), ( 0,  1,  0), ( 0,  1,  0), ( 0,  1,  0),
               (-1,  0,  0), (-1,  0,  0), (-1,  0,  0), (-1,  0,  0)]
boxPoints = [ ( h, -h, -h), (-h, -h, -h), ( h,  h, -h), (-h,  h, -h),
              ( h,  h,  h), (-h,  h,  h), (-h, -h,  h), ( h, -h,  h),
              ( h, -h,  h), (-h, -h,  h), (-h, -h, -h), ( h, -h, -h),
              ( h,  h,  h), ( h, -h,  h), ( h, -h, -h), ( h,  h, -h),
              (-h,  h,  h), ( h,  h,  h), ( h,  h, -h), (-h,  h, -h),
              (-h, -h,  h), (-h,  h,  h), (-h,  h, -h), (-h, -h, -h) ]
boxUVs = [ (0, 0), (0, 1), (1, 1), (1, 0),
           (0, 0), (0, 1), (1, 1), (1, 0),
           (0, 0), (0, 1), (1, 1), (1, 0),
           (0, 0), (0, 1), (1, 1), (1, 0),
           (0, 0), (0, 1), (1, 1), (1, 0),
           (0, 0), (0, 1), (1, 1), (1, 0) ]

def saveStage(stageUrl):
    global stage
    stage.GetRootLayer().Save()
    omni.client.usd_live_process()

def createBox(stageUrl, boxNumber=0):
    global stage
    rootUrl = '/Root'
    boxUrl = rootUrl + '/box_%d' % boxNumber

    xformPrim = UsdGeom.Xform.Define(stage, rootUrl)
    # Define the defaultPrim as the /Root prim
    rootPrim = stage.GetPrimAtPath(rootUrl)
    stage.SetDefaultPrim(rootPrim)

    boxPrim = UsdGeom.Mesh.Define(stage, boxUrl)

    boxPrim.CreateDisplayColorAttr([(0.463, 0.725, 0.0)])
    boxPrim.CreatePointsAttr(boxPoints)
    boxPrim.CreateNormalsAttr(boxNormals)
    boxPrim.CreateFaceVertexCountsAttr(boxVertexCounts)
    boxPrim.CreateFaceVertexIndicesAttr(boxVertexIndices)
    texCoords = boxPrim.CreatePrimvar("st", Sdf.ValueTypeNames.TexCoord2fArray, UsdGeom.Tokens.varying)
    texCoords.Set(boxUVs)
    texCoords.SetInterpolation("vertex")

    if not boxPrim:
        sys.exit("[ERROR] Failure to create box")
    
    saveStage(stageUrl)

    return boxPrim

def findGeomMesh(existingStage, boxNumber=0):
    global stage
    print(existingStage)

    stage = Usd.Stage.Open(existingStage)

    if not stage:
        sys.exit("[ERROR] Unable to open stage" + existingStage)

    #meshPrim = stage.GetPrimAtPath('/Root/box_%d' % boxNumber)
    for node in stage.Traverse():
        if node.IsA(UsdGeom.Mesh):
            return UsdGeom.Mesh(node)

    sys.exit("[ERROR] No UsdGeomMesh found in stage:", existingStage)
    return None


def uploadMaterial(destinationPath):
    uriPath = destinationPath + "/Materials"
    omni.client.delete(uriPath)
    omni.client.copy("resources/Materials", uriPath)


def createMaterial(mesh, stageUrl):
    # Create a material instance for this in USD
    materialName = "Fieldstone"
    newMat = UsdShade.Material.Define(stage, "/Root/Looks/Fieldstone")

    matPath = '/Root/Looks/Fieldstone'

    # MDL Shader
    # Create the MDL shader
    mdlShader = UsdShade.Shader.Define(stage, matPath+'/Fieldstone')
    mdlShader.CreateIdAttr("mdlMaterial")

    mdlShaderModule = "./Materials/Fieldstone.mdl"
    mdlShader.SetSourceAsset(mdlShaderModule, "mdl")
    mdlShader.GetPrim().CreateAttribute("info:mdl:sourceAsset:subIdentifier", Sdf.ValueTypeNames.Token, True).Set(materialName)

    mdlOutput = newMat.CreateSurfaceOutput("mdl")
    mdlOutput.ConnectToSource(mdlShader, "out")

    # USD Preview Surface Shaders

    # Create the "USD Primvar reader for float2" shader
    primStShader = UsdShade.Shader.Define(stage, matPath+'/PrimST')
    primStShader.CreateIdAttr("UsdPrimvarReader_float2")
    primStShader.CreateOutput("result", Sdf.ValueTypeNames.Float2)
    primStShader.CreateInput("varname", Sdf.ValueTypeNames.Token).Set("st")

    # Create the "Diffuse Color Tex" shader
    diffuseColorShader = UsdShade.Shader.Define(stage, matPath+'/DiffuseColorTex')
    diffuseColorShader.CreateIdAttr("UsdUVTexture")
    texInput = diffuseColorShader.CreateInput("file", Sdf.ValueTypeNames.Asset)
    texInput.Set("./Materials/Fieldstone/Fieldstone_BaseColor.png")
    texInput.GetAttr().SetColorSpace("RGB")
    diffuseColorShader.CreateInput("st", Sdf.ValueTypeNames.Float2).ConnectToSource(primStShader.CreateOutput("result", Sdf.ValueTypeNames.Float2))
    diffuseColorShaderOutput = diffuseColorShader.CreateOutput("rgb", Sdf.ValueTypeNames.Float3)

    # Create the "Normal Tex" shader
    normalShader = UsdShade.Shader.Define(stage, matPath+'/NormalTex')
    normalShader.CreateIdAttr("UsdUVTexture")
    normalTexInput = normalShader.CreateInput("file", Sdf.ValueTypeNames.Asset)
    normalTexInput.Set("./Materials/Fieldstone/Fieldstone_N.png")
    normalTexInput.GetAttr().SetColorSpace("RAW")
    normalShader.CreateInput("st", Sdf.ValueTypeNames.Float2).ConnectToSource(primStShader.CreateOutput("result", Sdf.ValueTypeNames.Float2))
    normalShaderOutput = normalShader.CreateOutput("rgb", Sdf.ValueTypeNames.Float3)

    # Create the USD Preview Surface shader
    usdPreviewSurfaceShader = UsdShade.Shader.Define(stage, matPath+'/PreviewSurface')
    usdPreviewSurfaceShader.CreateIdAttr("UsdPreviewSurface")
    diffuseColorInput = usdPreviewSurfaceShader.CreateInput("diffuseColor", Sdf.ValueTypeNames.Color3f)
    diffuseColorInput.ConnectToSource(diffuseColorShaderOutput)
    normalInput = usdPreviewSurfaceShader.CreateInput("normal", Sdf.ValueTypeNames.Normal3f)
    normalInput.ConnectToSource(normalShaderOutput)

    # Set the linkage between material and USD Preview surface shader
    usdPreviewSurfaceOutput = newMat.CreateSurfaceOutput()
    usdPreviewSurfaceOutput.ConnectToSource(usdPreviewSurfaceShader, "surface")

    UsdShade.MaterialBindingAPI(mesh).Bind(newMat)

    saveStage(stageUrl)


# Create a distant light in the scene.
def createDistantLight(stageUrl):
    newLight = UsdLux.DistantLight.Define(stage, "/Root/DistantLight")
    newLight.CreateAngleAttr(0.53)
    newLight.CreateColorAttr(Gf.Vec3f(1.0, 1.0, 0.745))
    newLight.CreateIntensityAttr(5000.0)

    saveStage(stageUrl)


# Create a dome light in the scene.
def createDomeLight(stageUrl, texturePath):
    newLight = UsdLux.DomeLight.Define(stage, "/Root/DomeLight")
    newLight.CreateIntensityAttr(1000.0)
    newLight.CreateTextureFileAttr(texturePath)
    newLight.CreateTextureFormatAttr("latlong")

    # Set rotation on domelight
    xForm = newLight
    rotateOp = xForm.AddXformOp(UsdGeom.XformOp.TypeRotateZYX, UsdGeom.XformOp.PrecisionFloat)
    rotateOp.Set(Gf.Vec3f(270, 0, 0))

    saveStage(stageUrl)


def createEmptyFolder(emptyFolderPath):
    print("Creating new folder: " + emptyFolderPath)
    result = omni.client.create_folder(emptyFolderPath)
    
    print("Finished [" + result.name + "]")

# from https://stackoverflow.com/questions/510357/how-to-read-a-single-character-from-the-user
def getChar():
    # figure out which function to use once, and store it in _func
    if "_func" not in getChar.__dict__:
        try:
            # for Windows-based systems
            import msvcrt # If successful, we are on Windows
            getChar._func=msvcrt.getch

        except ImportError:
            # for POSIX-based systems (with termios & tty support)
            import tty, sys, termios # raises ImportError if unsupported

            def _ttyRead():
                fd = sys.stdin.fileno()
                oldSettings = termios.tcgetattr(fd)

                try:
                    tty.setcbreak(fd)
                    answer = sys.stdin.read(1)
                finally:
                    termios.tcsetattr(fd, termios.TCSADRAIN, oldSettings)

                return answer.encode("ASCII")

            getChar._func=_ttyRead

    return getChar._func()

def liveEdit(meshIn, stageUrl):
    angle = 0
    omni.client.usd_live_wait_for_pending_updates()
    print("Begin Live Edit - Press 't' to move the box\nPress 'q' or escape to quit\n")

    while (True):
        option = getChar()
        omni.client.usd_live_wait_for_pending_updates()
        if option == b't':
            angle = (angle + 15) % 360
            radians = angle * 3.1415926 / 180.0
            x = math.sin(radians) * 100.0
            y = math.cos(radians) * 100.0

            xForm = meshIn

            position = Gf.Vec3d(0, 0, 0)
            rotZYX = Gf.Vec3f(0, 0, 0)
            scale = Gf.Vec3f(1, 1, 1)

            translateOp = None
            rotateOp = None
            scaleOp = None

            xFormOps = xForm.GetOrderedXformOps()
            for xop in xFormOps:
                if xop.GetOpType() == UsdGeom.XformOp.TypeTranslate:
                    translateOp = xop
                    position = translateOp.Get()
                if xop.GetOpType() == UsdGeom.XformOp.TypeRotateZYX:
                    rotateOp = xop
                    rotZYX = rotateOp.Get()
                if xop.GetOpType() == UsdGeom.XformOp.TypeScale:
                    scaleOp = xop
                    scale = scaleOp.Get()

            position += Gf.Vec3d(x, 0.0, y)
            rotZYX = Gf.Vec3f(rotZYX[0], angle, rotZYX[2])

            def setOp(xForm, op, opType, value, precision):
                if not op:
                    op = xForm.AddXformOp(opType, precision)
                    print(" Adding ", UsdGeom.XformOp.GetOpTypeToken(opType))

                if op.GetPrecision() == UsdGeom.XformOp.PrecisionFloat:
                    op.Set(Gf.Vec3f(value))
                else:
                    op.Set(value)

                print(" Setting ", UsdGeom.XformOp.GetOpTypeToken(opType))

                return op

            translateOp = setOp(xForm, translateOp, UsdGeom.XformOp.TypeTranslate, position, UsdGeom.XformOp.PrecisionDouble)
            rotateOp = setOp(xForm, rotateOp, UsdGeom.XformOp.TypeRotateZYX, rotZYX, UsdGeom.XformOp.PrecisionFloat)
            scaleOp = setOp(xForm, scaleOp, UsdGeom.XformOp.TypeScale, scale, UsdGeom.XformOp.PrecisionFloat)

            ops = [translateOp, rotateOp, scaleOp]

            xForm.SetXformOpOrder(ops)

            saveStage(stageUrl)

        elif option == b'q' or option == chr(27).encode():
            print("Live edit complete")
            break
        else:
            print("Enter 't' to transform or 'q' to quit.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Python Omniverse Client Sample",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument("-l", "--live", action='store_true', default=False)
    parser.add_argument("-p", "--path", action="store", default="omniverse://localhost/Users/test")
    parser.add_argument("-v", "--verbose", action='store_true', default=False)
    parser.add_argument("-e", "--existing", action="store")

    args = parser.parse_args()

    existingStage = args.existing
    doLiveEdit = args.live or bool(existingStage)
    destinationPath = args.path
    loggingEnabled = args.verbose

    startOmniverse(doLiveEdit)

    if destinationPath and not isValidOmniUrl(destinationPath):
        print("This is not an Omniverse Nucleus URL: " + destinationPath)
        print("Correct Omniverse URL format is: omniverse://server_name/Path/To/Example/Folder")
        print("Allowing program to continue because file paths may be provided in the form: C:/Path/To/Stage")

    if existingStage and not isValidOmniUrl(existingStage):
        print("This is not an Omniverse Nucleus URL: " + existingStage)
        print("Correct Omniverse URL format is: omniverse://server_name/Path/To/Example/Folder/helloWorld_py.usd")
        print("Allowing program to continue because file paths may be provided in the form: C:/Path/To/Stage/helloWorld_py.usd")

    boxMesh = None 

    if not existingStage:
        # Create the USD model in Omniverse
        stageUrl = createOmniverseModel(destinationPath)

        # Print the username for the server
        printConnectedUsername(stageUrl)

        # Create box geometry in the model
        boxMesh = createBox(stageUrl)

        # Add a Nucleus Checkpoint to the stage
        checkpointFile(stageUrl, "Add box and nothing else")

        # Create a distance and dome light in the scene
        createDistantLight(stageUrl)
        createDomeLight(stageUrl, "./Materials/kloofendal_48d_partly_cloudy.hdr")

        # Add a Nucleus Checkpoint to the stage
        checkpointFile(stageUrl, "Add lights to stage")

        # Upload a material and textures to the Omniverse server
        uploadMaterial(destinationPath)

        # Add a material to the box
        createMaterial(boxMesh, stageUrl)

        # Add a Nucleus Checkpoint to the stage
        checkpointFile(stageUrl, "Add material to the box")

        # Create an empty folder, just as an example
        createEmptyFolder(destinationPath + "/EmptyFolder")
    else:
        stageUrl = existingStage
        print("Stage url:", stageUrl)
        boxMesh = findGeomMesh(existingStage)

    if not boxMesh:
        sys.exit("[ERROR] Unable to create or find mesh")
    else:
        print("Mesh created/found successfully")

    if doLiveEdit and boxMesh is not None:
        liveEdit(boxMesh, stageUrl)

    shutdownOmniverse()
