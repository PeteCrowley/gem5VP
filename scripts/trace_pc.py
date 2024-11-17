file_name="m5out/trace_full.out"
out_file="m5out/trace_pc.out"
out_file2="m5out/trace_seq.out"

lines = open(file_name).readlines()

pc_lines = [0]
seq_nums = [0]

for line in lines:
    if "Committing instruction with PC" in line:
        pc = line[line.index("(")+1:line.index("=")]
        seq_num = line.split(" ")[3].strip()
        pc_lines.append(pc)
        seq_nums.append(seq_num)

# write pc_linex to new file
with open(out_file, 'w') as f:
    for item in pc_lines:
        f.write("%s\n" % item)

with open(out_file2, 'w') as f:
    for item in seq_nums:
        f.write("%s\n" % item)
