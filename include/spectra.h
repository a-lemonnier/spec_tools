#ifndef SPECTRA_H
#define SPECTRA_H


class _spectra
{
public:

    _spectra();

    _spectra(const _spectra& other);

    ~_spectra();

    _spectra& operator=(const _spectra& other);

    bool operator==(const _spectra& other) const;

    bool operator!=(const _spectra& other) const;

};

#include "spectra.tpp"

#endif // SPECTRA_H
