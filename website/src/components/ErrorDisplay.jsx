import React from 'react';
import { AlertCircle, X } from 'lucide-react';
import '../styles/error.css';

const ErrorDisplay = ({ message, onDismiss }) => {
  return (
    <div className="error-wrapper">
      <div className="error-box">
        <AlertCircle size={24} />
        <div className="error-text">
          <h4>Error</h4>
          <p>{message}</p>
        </div>
        {onDismiss && (
          <button className="error-close" onClick={onDismiss}>
            <X size={18} />
          </button>
        )}
      </div>
    </div>
  );
};

export default ErrorDisplay;
