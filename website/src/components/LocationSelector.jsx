import React, { useState } from 'react';
import { Search, Check } from 'lucide-react';
import '../styles/location-selector.css';

const LocationSelector = ({ locations = [], selectedLocations, onLocationToggle, disabled }) => {
  const [search, setSearch] = useState('');

  const filtered = locations.filter(loc =>
    loc.toLowerCase().includes(search.toLowerCase())
  );

  return (
    <div className="location-selector">
      <div className="search-wrapper">
        <Search size={16} />
        <input
          type="text"
          placeholder="Search..."
          value={search}
          onChange={(e) => setSearch(e.target.value)}
          disabled={disabled}
        />
      </div>

      <div className="locations-scroll">
        {filtered.map((location) => {
          const isSelected = selectedLocations.includes(location);
          const order = selectedLocations.indexOf(location) + 1;

          return (
            <div
              key={location}
              className={`location ${isSelected ? 'selected' : ''} ${disabled ? 'disabled' : ''}`}
              onClick={() => !disabled && onLocationToggle(location)}
            >
              <div className="checkbox">
                {isSelected && <Check size={12} />}
              </div>
              <span className="name">{location}</span>
              {isSelected && <span className="order">{order}</span>}
            </div>
          );
        })}
      </div>
    </div>
  );
};

export default LocationSelector;
