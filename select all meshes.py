import bpy


def getchildren(obj):
    children = []
    for child in obj.children:
        if child and child.type == 'MESH':
            children.append(child)
        
        children.extend(getchildren(child))
    return children

Map = bpy.data.objects.get("Map")
mapchildren = getchildren(Map)

# Deselect everything first
bpy.ops.object.select_all(action='DESELECT')

# Select objects that are enabled in renders
for obj in mapchildren:
    if not obj.get('IsKillbox') and not obj.get('Hitbox') and obj.type == 'MESH':
        obj.select_set(True)
