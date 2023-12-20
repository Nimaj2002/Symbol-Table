import json
from graphviz import Digraph


def draw_tree(data):
    dot = Digraph(comment='The Round Table')
    type_dict = {0: "bool", 1: "int", 2: "float", 3: "char"}

    for key, value in data.items():
        if value["prev"] != "null":
            dot.edge(value["prev"], key, arrowhead="none")

    for key, value in data.items():
        if (len(value) > 1):
            label = "{"
            label += f"<TR><TD>Block</TD><TD>{key}</TD></TR>"
            for k, v in value.items():
                if k != "prev":
                    label += f"<TR><TD>{k}</TD><TD>{type_dict[v]}</TD></TR>"
            label += "}"
            dot.node(
                key, f'''<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0">{label}</TABLE>>''')
        else:
            label = "{"
            label += f"<TR><TD>Block</TD><TD>{key}</TD></TR>"
            label += f"<TR><TD>ND</TD></TR>"
            label += "}"
            dot.node(
                key, f'''<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0">{label}</TABLE>>''')

    dot.render('tableVisualizer/test-output/tree.gv', view=True)


if __name__ == "__main__":
    # Open the JSON file
    with open('tableVisualizer/data.json', 'r') as file:
        json_data = json.load(file)
    draw_tree(json_data)
