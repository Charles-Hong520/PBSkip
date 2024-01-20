# turns generates code that turns PBS message to Google's PB message
# used for testing the correctness of PBS parsing. 
import os
import sys
import re

# read the .proto file as lines
filename = input("enter filename without extension: ")


def read_proto_file(file_name):
    lines = []
    with open(file_name, "r") as f:
        file = f.readlines()
        for line in file:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            lines.append(line)
    f.close()
    return lines


# parse the .proto file and return the class name and the fields
global_classes = {}
prototype_to_cpptype = {
    "uint32": "uint32_t",
    "uint64": "uint64_t",
    "int32": "int32_t",
    "int64": "int64_t",
    "bool": "bool",
    "string": "std::string",
}
wire0 = {"uint32", "int32", "uint64", "int64", "bool"}
wire2 = {"std::string"}


def parse_proto_file(lines):
    classes = {}
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("message"):
            class_name = line.split(" ")[1].strip()
            fields = []
            attr = []
            i += 1
            while not lines[i].startswith("}"):
                fields.append(lines[i].strip())
                curr = lines[i].strip().split()
                attr.append({})
                attr[-1]["repeated"] = curr[0] == "repeated"
                offset = 1 if attr[-1]["repeated"] else 0
                attr[-1]["proto_type"] = curr[0 + offset]
                attr[-1]["var"] = curr[1 + offset]
                attr[-1]["field_id"] = curr[3 + offset][:-1]
                if attr[-1]["proto_type"] in prototype_to_cpptype:
                    attr[-1]["cpp_type"] = prototype_to_cpptype[attr[-1]["proto_type"]]
                else:
                    attr[-1]["cpp_type"] = attr[-1]["proto_type"] + "*"
                i += 1
            classes[class_name] = fields
            global_classes[class_name] = attr
        i += 1
    # {classname: [(repeated) type var = field_id]}
    return classes


classes = parse_proto_file(read_proto_file("schema/" + filename + ".proto"))

# print(global_classes)

file = open("sequential/" + filename + "_pbsTopb" + ".h", "w")
pkg = "pp::"


def generate_Class(classname, src, dst, depth):
    string = ""
    for i in global_classes[classname]:
        if i["repeated"] and "*" in i["cpp_type"]:
            string += f"""for (auto e{depth} : {src}->get_{i['var']}()) {{
{pkg}{i['cpp_type']} temp{depth} = {dst}->add_{i['var'].lower()}();
"""
            string += generate_Class(
                i["proto_type"], f"e{depth}", f"temp{depth}", depth + 1
            )
            string += "}\n"
        elif i["repeated"] and "*" not in i["cpp_type"]:
            string += f"""for (auto e{depth} : {src}->get_{i['var']}()) {{
{dst}->add_{i['var'].lower()}(e{depth});
}}
"""
        elif not i["repeated"] and "*" in i["cpp_type"]:
            string += f"""{pkg}{i['cpp_type']} temp{depth} = new {pkg}{i['proto_type']}();
{generate_Class(i["proto_type"], f"{src}->get_{i['var']}()", f"temp{depth}", depth+1)}
{dst}->set_allocated_{i['var'].lower()}(temp{depth});
"""
        elif not i["repeated"] and "*" not in i["cpp_type"]:
            string += f"""{dst}->set_{i['var'].lower()}({src}->get_{i['var']}());
"""
    return string


string = generate_Class(f"{filename.capitalize()}", "pbs", "profile_custom", 0)
file.write(string)
file.close()
