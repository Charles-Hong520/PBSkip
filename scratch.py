import re

file = open("times.txt", "r")

capture_lines = False  # Flag to indicate whether to capture lines after "---"
captured_lines = []  # List to store captured lines
data_dict = {}
for line in file:
    line = line.strip()  # Remove leading/trailing whitespace
    if line.startswith("---"):
        # If a line starts with "---", set the flag to capture lines
        line = line.strip("-")
        line = line.replace("Running the ", "")
        line = line.replace("version", "")
        line = line.replace("verison", "")
        line = line.strip()
        capture_lines = True
        captured_lines = [line]  # Reset the list of captured lines
    elif capture_lines:
        pattern = r"(\w+): ([\d.]+)"

        # Use re.findall() to extract key-value pairs from the line
        matches = re.findall(pattern, line)

        # Initialize an empty dictionary to store the parsed data
        line_dict = {}

        # Iterate through the matches and store them in the dictionary
        for key, value in matches:
            line_dict[key] = value
        captured_lines.append(line_dict)

        # Check if we have captured 10 lines after "---"
        if len(captured_lines) == 11:
            # Create a key using the "---" line as the identifier
            key = captured_lines[0]
            # Store the captured lines (10 lines) in the dictionary
            data_dict[key] = captured_lines[1:]
            capture_lines = False  # Reset the flag
            captured_lines = []  # Reset the list

# Print the parsed data
outfile = open("output.txt", "w")
for name, lines in data_dict.items():
    totalTime = 0
    totalMaster = 0
    for line in lines:
        if "time" in line:
            totalTime += float(line["time"])
        if "master" in line:
            totalMaster += float(line["master"])
    outfile.write(
        f"{name}:\t{totalTime/len(lines)}\t{totalMaster/len(lines) if totalMaster != 0 else ''}\n"
    )
