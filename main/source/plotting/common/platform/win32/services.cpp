#include <plotting/common/services.h>
#include <stdio.h>
#include <memory>

namespace
{
    class desktop_services: public services
    {
        virtual stream* open_file(const char* path);
    };

    stream* desktop_services::open_file(const char* path)
    {
        class file_stream: public stream
        {
        public:
            file_stream(const char* path)
                : _stream(fopen(path, "rb"), &fclose)
            {}

            virtual void read(void* buffer, size_t size)
            {
                fread(buffer, 1, size, _stream.get());
            }

        private:
            std::shared_ptr<FILE> _stream;
        };

        return new file_stream(path);
    }
}


services& get_services()
{
    static desktop_services serv;
    return serv;
}