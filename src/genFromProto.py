# generate only 1 .pbs.h and .pbs.cpp file for each .proto file that follows the naming convention and pattern from ./sequencial folder for person

import os
import sys
import re

# read the .proto file as lines

filename = "person"


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
                attr[-1]["field_id"] = curr[-1][:-1]
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


# generate the .pbs.h file
def generate_H(classes):
    # open the .pbs.h file and write the header to it
    with open("generated/" + filename + ".pbs.h", "w") as f:
        name = "__" + filename.upper() + "_H__"
        f.write("#ifndef " + name + "\n")
        f.write("#define " + name + "\n")
        f.write("#include <string>\n")
        f.write("#include <vector>\n")
        f.write("#include <iostream>\n")
        f.write('#include "../lib.h"\n')

        # write the class name and the fields to the .pbs.h file
        for class_name, fields in classes.items():
            f.write("class " + class_name + " {\n")
            field_type = ""
            field_name = ""
            members = (
                []
            )  # to store the names of the members of the class and their types

            for field in fields:
                # first split the field into type and name
                field = field.split(" ")
                if field[0] == "repeated":
                    field_type = field[1]
                    field_name = field[2]
                else:
                    field_type = field[0]
                    field_name = field[1]
                if field_type == "string":
                    field_type = "std::string"
                elif field_type == "int32":
                    field_type = "int32_t"
                elif field_type == "int64":
                    field_type = "int64_t"
                elif field_type == "uint32":
                    field_type = "uint32_t"
                elif field_type == "uint64":
                    field_type = "uint64_t"
                # if the field is a class name from another member then type is the class name pointer
                elif field_type in classes.keys():
                    field_type = field_type + "*"

                members.append(
                    (field_type, field_name, True if field[0] == "repeated" else False)
                )

            private_members = []
            public_members = []
            temp_string = "const std::string&"
            public_members.append("\t" + class_name + "();\n")
            # for member in members:
            #     if member[0] == "std::string":
            #         temp = '"";\n'
            #     elif (
            #         member[0] == "int32_t"
            #         or member[0] == "int64_t"
            #         or member[0] == "uint32_t"
            #         or member[0] == "uint64_t"
            #     ):
            #         temp = "0;\n"
            #     elif member[0] == "bool":
            #         temp = "false;\n"
            #     elif member[0] == class_name + "*":
            #         temp = "nullptr;\n"
            #     if member[2] == False:
            #         public_members.append("\t\t" + member[1] + " = " + temp)
            # public_members.append("\t}\n")
            for member in members:
                if member[2] == True:
                    private_members.append(
                        "\tstd::vector<" + member[0] + "> " + member[1] + ";\n"
                    )
                    if member[0] == "std::string":
                        public_members.append(
                            "\tvoid add_" + member[1] + "(" + temp_string + ");\n"
                        )
                    else:
                        public_members.append(
                            "\tvoid add_" + member[1] + "(" + member[0] + ");\n"
                        )
                else:
                    private_members.append("\t" + member[0] + " " + member[1] + ";\n")
                    if member[0] == "std::string":
                        public_members.append(
                            "\tvoid set_" + member[1] + "(" + temp_string + ");\n"
                        )
                    else:
                        public_members.append(
                            "\tvoid set_" + member[1] + "(" + member[0] + ");\n"
                        )

            # write private and public members to the file
            f.write("private:\n")
            for mem in private_members:
                f.write(mem)
            f.write("public:\n")
            public_members.append("\tbool parse" + class_name + "(Seeker&);\n")
            public_members.append(
                "\tfriend std::ostream& operator<<(std::ostream& os, const "
                + class_name
                + "&"
                + ");\n"
            )
            for mem in public_members:
                f.write(mem)

            f.write("};\n")

        f.write("#endif\n")
    f.close()


def read_H_file(file_name):
    lines = []
    with open("generated/" + file_name + ".pbs.h", "r") as f:
        file = f.readlines()
        for line in file:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            lines.append(line)
    f.close()
    return lines


def genCase(f):
    string = ""
    p_type = f["proto_type"]
    if p_type in wire0 and not f["repeated"]:
        bits = "32"
        if "64" in p_type:
            bits = "64"
        string = f"""\t\tcase {f["field_id"]}:
\t\t\tif (wire == 0) {{
\t\t\t\tuint{bits}_t i;
\t\t\t\tseek.ReadVarint{bits}(&i);
\t\t\t\tset_{f["var"]}(i);
\t\t\t}}
\t\t\tbreak;
"""
    elif p_type in wire0 and f["repeated"]:
        bits = "32"
        if "64" in p_type:
            bits = "64"
        string = f"""\t\tcase {f["field_id"]}:
\t\tif (wire == 2) {{
\t\t\tuint32_t len;
\t\t\tseek.ReadVarint32(&len);
\t\t\tuint{bits}_t i;
\t\t\twhile (len--) {{
\t\t\t\tseek.ReadVarint{bits}(&i);
\t\t\t\tadd_{f["var"]}(i);
\t\t\t}}
\t\t}}
\t\tbreak;
"""
    elif p_type == "string":
        string = f"""\t\tcase {f["field_id"]}:
\t\tif (wire == 2) {{
\t\t\tuint32_t len;
\t\t\tseek.ReadVarint32(&len);
\t\t\tstd::string buffer;
\t\t\tseek.ReadString(&buffer, len);
\t\t\t{"add" if f["repeated"] else "set"}_{f["var"]}(buffer);
\t\t}}
\t\tbreak;
"""
    elif "*" in f["cpp_type"]:
        string = f"""\t\tcase {f["field_id"]}:
\t\tif (wire == 2) {{
\t\t\tuint32_t len;
\t\t\tseek.ReadVarint32(&len);
\t\t\t{f["cpp_type"]} msg_ = new {f["proto_type"]}();
\t\t\tSeeker copyseeker(seek, len);
\t\t\tseek.curr += len;
\t\t\tmsg_->parseAddress(copyseeker);
\t\t\t{"add" if f["repeated"] else "set"}_{f["var"]}(msg_);
\t\t}}
\t\tbreak;
"""
    print(string)
    return string


def parseClass(classname, fields):
    # input: {classname: [{'repeated': False, 'proto_type': 'string', 'var': 'name', 'field_id': '1', 'cpp_type': 'std::string'}]}

    string = f"""bool {classname}::parse{classname}(Seeker& seek) {{
\tuint32_t tag = seek.ReadTag();
\twhile (tag != 0) {{
\t\tuint32_t field_id = tag >> 3;
\t\tuint32_t wire = tag & 7;

\t\tswitch (field_id) {{
"""
    for f in fields:
        string += genCase(f)

    string += f"""\t\t}}
\t}}
}}
"""
    return string


def generate_CPP(lines):
    with open("generated/" + filename + ".pbs.cpp", "w") as f:
        f.write('#include "' + filename + '.pbs.h"\n')
        classes = {}
        i = 0
        while i < len(lines):
            if lines[i].startswith("class"):
                class_name = lines[i].split(" ")[1].strip()
                classes[class_name] = []
                i += 1
                if lines[i].startswith("private:"):
                    i += 1
                while not lines[i].startswith("public:"):
                    type_name = lines[i].split(" ")[0].strip()
                    name = lines[i].split(" ")[1].strip()
                    name = name[:-1]
                    classes[class_name].append((type_name, name))
                    i += 1
                if lines[i].startswith("public:"):
                    i += 1
                f.write(class_name + "::" + class_name + "() {\n")
                temp = ""
                for type_name, name in classes[class_name]:
                    if type_name == "std::string":
                        temp = '"";\n'
                    elif (
                        type_name == "int32_t"
                        or type_name == "int64_t"
                        or type_name == "uint32_t"
                        or type_name == "uint64_t"
                    ):
                        temp = "0;\n"
                    elif type_name == "bool":
                        temp = "false;\n"
                    elif "*" in type_name:
                        temp = "nullptr;\n"
                    if not type_name.startswith("std::vector"):
                        f.write("\t" + name + " = " + temp)
                f.write("}\n")
                while not lines[i].startswith("};"):
                    if lines[i].startswith(class_name) or lines[i].startswith("friend"):
                        i += 1
                        continue
                    elif f"parse{class_name}" in lines[i]:
                        # TODO: parse function
                        f.write(parseClass(class_name, global_classes[class_name]))
                        i += 1
                        continue
                    lines[i] = lines[i].split(" ")
                    lines[i][1] = class_name + "::" + lines[i][1]
                    lines[i] = " ".join(lines[i])
                    l = lines[i].find("_")
                    r = lines[i].find("(")
                    var = lines[i][l + 1 : r]
                    lines[i] = lines[i][:-2] + " " + var + ") {\n"
                    f.write(lines[i])
                    if lines[i].startswith(f"void {class_name}::add"):
                        f.write("\t" + "this->" + var + ".push_back(" + var + ");\n")
                    elif lines[i].startswith(f"void {class_name}::set"):
                        f.write("\t" + "this->" + var + " = " + var + ";\n")
                    f.write("}\n")
                    i += 1
            i += 1
        # print(classes)


classes = parse_proto_file(read_proto_file("schema/" + filename + ".proto"))
generate_H(classes)

generate_CPP(read_H_file(filename))

# for key, val in global_classes.items():
#     print(key)
#     for e in val:
#         print("\n", e)
