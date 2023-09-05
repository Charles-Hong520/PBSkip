repeated2msg = {
    1: ("ValueType", "sample_type"),
    2: ("Sample", "sample"),
    3: ("Mapping", "mapping"),
    4: ("Location", "location"),
    5: ("Function", "function"),
}
repeated2str = {
    6: ("std::string", "string_table"),
}
filename = "bulk_gen.txt"


def parallel():
    file = open(filename, "w")
    str = ""
    for fid, sec in repeated2msg.items():
        (t, var) = sec
        str += f"""
  void Profile::bulk_add_{var}(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {{
    parlay::parallel_for(0, tr.size(), [&](int i) {{
      Seeker seeker(b, tr[i].first, tr[i].second);
      {var}[i] = new {t}();
      {var}[i]->parse{t}(seeker);
    }});
  }}
  """
    for fid, sec in repeated2str.items():
        (t, var) = sec
        str += f"""
void Profile::bulk_add_{var}(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {{
  parlay::parallel_for(0, tr.size(), [&](int i) {{
    Seeker seeker(b, tr[i].first, tr[i].second);
    seeker.ReadString(&{var}[i], tr[i].second);
  }});
}}
"""
    file.write(str)
    file.close()


def sequential():
    file = open(filename, "w")
    str = ""
    for fid, sec in repeated2msg.items():
        (t, var) = sec
        str += f"""
  void Profile::bulk_add_{var}(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {{
  int sum = 0;
    for(int i = 0; i < tr.size(); i++) {{
      Seeker seeker(b, tr[i].first, tr[i].second);
      {var}[i] = new {t}();
      {var}[i]->parse{t}(seeker);
    }}
  }}
  """
    for fid, sec in repeated2str.items():
        (t, var) = sec
        str += f"""
void Profile::bulk_add_{var}(std::vector<std::pair<uint64_t, uint64_t>>& tr, uint8_t* b) {{
  int sum = 0;
  for(int i = 0; i < tr.size(); i++) {{
    Seeker seeker(b, tr[i].first, tr[i].second);
    seeker.ReadString(&{var}[i], tr[i].second);
  }}
}}
"""
    file.write(str)
    file.close()


sequential()
