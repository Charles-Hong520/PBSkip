# generate only 1 .pbs.h and .pbs.cpp file for each .proto file that follows the naming convention and pattern from ./sequencial folder for person

import os
import sys
import re

# read the .proto file as lines

filename = "profile"


def read_proto_file(file_name):
    lines = []
    with open(file_name, "r") as f:
        file = f.readlines()
        for line in file:
            line = line.strip()
            if not line or line.startswith("//"):
                continue
            lines.append(line)
    return lines


# parse the .proto file and return the class name and the fields


def parse_proto_file(lines):
    classes = {}
    i = 0
    while i < len(lines):
        line = lines[i]
        if line.startswith("message"):
            class_name = line.split(" ")[1].strip()
            fields = []
            i += 1
            while not lines[i].startswith("}"):
                fields.append(lines[i].strip())
                i += 1
            classes[class_name] = fields
        i += 1
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
            public_members.append("\t" + class_name + "() {\n")
            for member in members:
                if member[0] == "std::string":
                    temp = '"";\n'
                elif (
                    member[0] == "int32_t"
                    or member[0] == "int64_t"
                    or member[0] == "uint32_t"
                    or member[0] == "uint64_t"
                ):
                    temp = "0;\n"
                elif member[0] == "bool":
                    temp = "false;\n"
                elif member[0] == class_name + "*":
                    temp = "nullptr;\n"
                if member[2] == False:
                    public_members.append("\t\t" + member[1] + " = " + temp)
            public_members.append("\t}\n")
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
                + "& "
                + class_name.lower()[0]
                + ");\n"
            )
            for mem in public_members:
                f.write(mem)

            f.write("};\n")

        f.write("#endif\n")


classes = parse_proto_file(read_proto_file("schema/" + filename + ".proto"))
generate_H(classes)
