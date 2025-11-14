import React from 'react';
import { Loader2 } from 'lucide-react';
import '../styles/loading.css';

const LoadingSpinner = () => {
  return (
    <div className="loading-wrapper">
      <Loader2 size={48} className="spinner" />
      <h3>Computing Route</h3>
      <p>Analyzing optimal path...</p>
    </div>
  );
};

export default LoadingSpinner;
