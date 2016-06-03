package adata;

public interface base {
    public abstract void read(adata.Stream stream) throws Exception;
    public abstract void skip_read(adata.Stream stream) throws Exception;
    public abstract int size_of();
    public abstract void write(adata.Stream stream)throws Exception;
    public abstract void raw_read(adata.Stream stream) throws Exception;
    public abstract int raw_size_of();
    public abstract void raw_write(adata.Stream stream)throws Exception;
}
