struct Basic {
    int eval_const_expr(const Expr& rpn);
    bool eval_expr(const Expr& rpn, int asmpc, int& value, bool do_error = true);
    void parse_b81_file(const string& filename);
    void write_b81_file(const string& filename);

private:
    void write_sysvars(ofstream& ofs) const;
    void write_basic_lines(ofstream& ofs) const;
    void write_video(ofstream& ofs) const;
    void write_basic_vars(ofstream& ofs) const;
    void write_basic_system(ofstream& ofs) const;
    void write_mem_info(ofstream& ofs, int start_addr, int len) const;
    void write_basic_memory_map(ofstream& ofs) const;
};
