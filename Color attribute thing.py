import bpy

# Loop through all objects in the current scene
for obj in bpy.data.objects:
    # Check if the object is a mesh
    if obj.type == 'MESH':
        mesh = obj.data
        # Define the name of your color attribute
        attr_name = "Color"
        
        # Check if the color attribute already exists
        if attr_name not in mesh.color_attributes:
            mesh.color_attributes.new(name=attr_name, type='FLOAT_COLOR', domain='POINT')
            print(f"Added color attribute to {obj.name}")
