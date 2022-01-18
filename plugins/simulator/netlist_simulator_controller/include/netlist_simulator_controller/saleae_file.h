#pragma once

#include <fstream>
#include <functional>
#include "netlist_simulator_controller/saleae_directory.h"

namespace hal
{
    class SaleaeStatus
    {
    public:
        enum ErrorCode { ErrorOpenFile = -4,
                         BadIndentifier = -3,
                         UnsupportedType = -2,
                         UnexpectedEof = -1,
                         Ok = 0};
    };

    class SaleaeHeader
    {
    public:
        enum StorageFormat { Double = 0, Uint64 = 0x206c6168, Coded = 0x786c6168 };
        char mIdent[9];
        int32_t mVersion;
        StorageFormat mStorageFormat;
        uint32_t mValue;
        uint64_t mBeginTime;
        uint64_t mEndTime;
        uint64_t mNumTransitions;

        static const char* sIdent;
    public:
        SaleaeHeader();

        SaleaeStatus::ErrorCode read(std::ifstream& ff);
        SaleaeStatus::ErrorCode write(std::ofstream& of) const;

        StorageFormat storageFormat() const { return mStorageFormat; }
        void setStorageFormat(StorageFormat sf) { mStorageFormat = sf; }

        uint32_t value() const { return mValue; }
        void setValue(uint32_t v) { mValue = v; }

        uint64_t beginTime() const { return mBeginTime; }
        void setBeginTime(uint64_t t) { mBeginTime = t; }

        uint64_t endTime() const { return mEndTime; }
        void setEndTime(uint64_t t) { mEndTime = t; }

        uint64_t numTransitions() const { return mNumTransitions; }
        void incrementTransitions() { ++mNumTransitions; }
    };

    class SaleaeDataBuffer
    {
    public:
        uint64_t mCount;
        uint64_t* mTimeArray;
        int* mValueArray;
        bool isNull() const { return mCount == 0; }
        SaleaeDataBuffer(uint64_t cnt = 0);
        ~SaleaeDataBuffer();
        void convertCoded();
        void dump() const;
    };

    class SaleaeDataTuple
    {
    public:
        uint64_t mTime;
        int mValue;
    };

    class SaleaeInputFile : public std::ifstream
    {
        SaleaeHeader mHeader;
        uint64_t mNumRead;
        SaleaeStatus::ErrorCode mStatus;

        std::function<uint64_t()> mReader;

    public:
        SaleaeInputFile(const std::string& filename);
        int startValue() const { return mHeader.value(); }

        SaleaeDataTuple nextValue(int lastValue);
        std::string get_last_error() const;
        SaleaeDataBuffer get_data();

        const SaleaeHeader* header() const { return &mHeader; }
    };


    class SaleaeOutputFile : public std::ofstream
    {
        int mIndex;
        std::string mFilename;
        SaleaeHeader mHeader;
        SaleaeStatus::ErrorCode mStatus;
        bool mFirstValue;

        void convertToCoded();
    public:
        SaleaeOutputFile(const std::string& filename, int index_);
        ~SaleaeOutputFile();
        void writeTimeValue(uint64_t t, int32_t val);
        void close();
        int index() const { return mIndex; }

        void put_data(SaleaeDataBuffer& buf);

        SaleaeDirectoryFileIndex fileIndex() const;
    };
}
